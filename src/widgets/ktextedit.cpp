/* This file is part of the KDE libraries
   Copyright (C) 2002 Carsten Pfeiffer <pfeiffer@kde.org>
                 2005 Michael Brade <brade@kde.org>
         2012 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "ktextedit.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QMenu>
#include <QScrollBar>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QDebug>
#ifdef HAVE_SPEECH
#include <QTextToSpeech>
#endif
#include <QSettings>

#include <sonnet/configdialog.h>
#include <sonnet/dialog.h>
#include <sonnet/backgroundchecker.h>
#include <sonnet/spellcheckdecorator.h>
#include <kcursor.h>
#include <kstandardaction.h>
#include <kicontheme.h>
#include <kstandardshortcut.h>
#include <klocalizedstring.h>
#include <kmessagebox.h>
#include <kwindowsystem.h>

#include "kreplacedialog.h"
#include "kfinddialog.h"
#include "kfind.h"
#include "kreplace.h"

class KTextDecorator : public Sonnet::SpellCheckDecorator
{
public:
    KTextDecorator(KTextEdit *textEdit);
    bool isSpellCheckingEnabledForBlock(const QString &textBlock) const Q_DECL_OVERRIDE;
private:
    KTextEdit *m_textEdit;
};

class KTextEdit::Private
{
public:
    Private(KTextEdit *_parent)
        : parent(_parent),
          languagesMenu(nullptr),
          customPalette(false),
          spellCheckingEnabled(false),
          findReplaceEnabled(true),
          showTabAction(true),
          showAutoCorrectionButton(false),
          decorator(nullptr), speller(nullptr), findDlg(nullptr), find(nullptr), repDlg(nullptr), replace(nullptr),
#ifdef HAVE_SPEECH
          textToSpeech(nullptr),
#endif
          findIndex(0), repIndex(0),
          lastReplacedPosition(-1)
    {
        //Check the default sonnet settings to see if spellchecking should be enabled.
        QSettings settings(QStringLiteral("KDE"), QStringLiteral("Sonnet"));
        spellCheckingEnabled = settings.value(QStringLiteral("checkerEnabledByDefault"), false).toBool();
#ifdef HAVE_SPEECH
        textToSpeech = new QTextToSpeech(parent);
#endif
    }

    ~Private()
    {
        delete decorator;
        delete findDlg;
        delete find;
        delete replace;
        delete repDlg;
        delete speller;
#ifdef HAVE_SPEECH
        delete textToSpeech;
#endif
    }

    /**
     * Checks whether we should/should not consume a key used as a shortcut.
     * This makes it possible to handle shortcuts in the focused widget before any
     * window-global QAction is triggered.
     */
    bool overrideShortcut(const QKeyEvent *e);
    /**
     * Actually handle a shortcut event.
     */
    bool handleShortcut(const QKeyEvent *e);

    void spellCheckerMisspelling(const QString &text, int pos);
    void spellCheckerCorrected(const QString &, int, const QString &);
    void spellCheckerAutoCorrect(const QString &, const QString &);
    void spellCheckerCanceled();
    void spellCheckerFinished();
    void toggleAutoSpellCheck();

    void slotFindHighlight(const QString &text, int matchingIndex, int matchingLength);
    void slotReplaceText(const QString &text, int replacementIndex, int /*replacedLength*/, int matchedLength);

    /**
     * Similar to QTextEdit::clear(), only that it is possible to undo this
     * action.
     */
    void undoableClear();

    void slotAllowTab();
    void menuActivated(QAction *action);

    void init();

    void checkSpelling(bool force);
    KTextEdit *parent;
    QAction *autoSpellCheckAction;
    QAction *allowTab;
    QAction *spellCheckAction;
    QMenu *languagesMenu;
    bool customPalette : 1;

    bool spellCheckingEnabled : 1;
    bool findReplaceEnabled: 1;
    bool showTabAction: 1;
    bool showAutoCorrectionButton: 1;
    QTextDocumentFragment originalDoc;
    QString spellCheckingLanguage;
    Sonnet::SpellCheckDecorator *decorator;
    Sonnet::Speller *speller;
    KFindDialog *findDlg;
    KFind *find;
    KReplaceDialog *repDlg;
    KReplace *replace;
#ifdef HAVE_SPEECH
    QTextToSpeech *textToSpeech;
#endif

    int findIndex, repIndex;
    int lastReplacedPosition;
};

void KTextEdit::Private::checkSpelling(bool force)
{
    if (parent->document()->isEmpty()) {
        KMessageBox::information(parent, i18n("Nothing to spell check."));
        if (force) {
            emit parent->spellCheckingFinished();
        }
        return;
    }
    Sonnet::BackgroundChecker *backgroundSpellCheck = new Sonnet::BackgroundChecker;
    if (!spellCheckingLanguage.isEmpty()) {
        backgroundSpellCheck->changeLanguage(spellCheckingLanguage);
    }
    Sonnet::Dialog *spellDialog = new Sonnet::Dialog(
        backgroundSpellCheck, force ? parent : nullptr);
    backgroundSpellCheck->setParent(spellDialog);
    spellDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    spellDialog->activeAutoCorrect(showAutoCorrectionButton);
    connect(spellDialog, SIGNAL(replace(QString,int,QString)),
            parent, SLOT(spellCheckerCorrected(QString,int,QString)));
    connect(spellDialog, SIGNAL(misspelling(QString,int)),
            parent, SLOT(spellCheckerMisspelling(QString,int)));
    connect(spellDialog, &Sonnet::Dialog::autoCorrect,
            parent, &KTextEdit::spellCheckerAutoCorrect);
    connect(spellDialog, SIGNAL(done(QString)),
            parent, SLOT(spellCheckerFinished()));
    connect(spellDialog, SIGNAL(cancel()),
            parent, SLOT(spellCheckerCanceled()));
    //Laurent in sonnet/dialog.cpp we emit done(QString) too => it calls here twice spellCheckerFinished not necessary
    /*
    connect(spellDialog, SIGNAL(stop()),
            parent, SLOT(spellCheckerFinished()));
    */
    connect(spellDialog, &Sonnet::Dialog::spellCheckStatus,
            parent, &KTextEdit::spellCheckStatus);
    connect(spellDialog, &Sonnet::Dialog::languageChanged,
            parent, &KTextEdit::languageChanged);
    if (force) {
        connect(spellDialog, SIGNAL(done(QString)), parent, SIGNAL(spellCheckingFinished()));
        connect(spellDialog, &Sonnet::Dialog::cancel, parent, &KTextEdit::spellCheckingCanceled);
        //Laurent in sonnet/dialog.cpp we emit done(QString) too => it calls here twice spellCheckerFinished not necessary
        //connect(spellDialog, SIGNAL(stop()), parent, SIGNAL(spellCheckingFinished()));
    }
    originalDoc = QTextDocumentFragment(parent->document());
    spellDialog->setBuffer(parent->toPlainText());
    spellDialog->show();
}

void KTextEdit::Private::spellCheckerCanceled()
{
    QTextDocument *doc = parent->document();
    doc->clear();
    QTextCursor cursor(doc);
    cursor.insertFragment(originalDoc);
    spellCheckerFinished();
}

void KTextEdit::Private::spellCheckerAutoCorrect(const QString &currentWord, const QString &autoCorrectWord)
{
    emit parent->spellCheckerAutoCorrect(currentWord, autoCorrectWord);
}

void KTextEdit::Private::spellCheckerMisspelling(const QString &text, int pos)
{
    //qDebug()<<"TextEdit::Private::spellCheckerMisspelling :"<<text<<" pos :"<<pos;
    parent->highlightWord(text.length(), pos);
}

void KTextEdit::Private::spellCheckerCorrected(const QString &oldWord, int pos, const QString &newWord)
{
    //qDebug()<<" oldWord :"<<oldWord<<" newWord :"<<newWord<<" pos : "<<pos;
    if (oldWord != newWord) {
        QTextCursor cursor(parent->document());
        cursor.setPosition(pos);
        cursor.setPosition(pos + oldWord.length(), QTextCursor::KeepAnchor);
        cursor.insertText(newWord);
    }
}

void KTextEdit::Private::spellCheckerFinished()
{
    QTextCursor cursor(parent->document());
    cursor.clearSelection();
    parent->setTextCursor(cursor);
    if (parent->highlighter()) {
        parent->highlighter()->rehighlight();
    }
}

void KTextEdit::Private::toggleAutoSpellCheck()
{
    parent->setCheckSpellingEnabled(!parent->checkSpellingEnabled());
}

void KTextEdit::Private::undoableClear()
{
    QTextCursor cursor = parent->textCursor();
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
    cursor.endEditBlock();
}

void KTextEdit::Private::slotAllowTab()
{
    parent->setTabChangesFocus(!parent->tabChangesFocus());
}

void KTextEdit::Private::menuActivated(QAction *action)
{
    if (action == spellCheckAction) {
        parent->checkSpelling();
    } else if (action == autoSpellCheckAction) {
        toggleAutoSpellCheck();
    } else if (action == allowTab) {
        slotAllowTab();
    }
}

void KTextEdit::Private::slotFindHighlight(const QString &text, int matchingIndex, int matchingLength)
{
    Q_UNUSED(text)
    //qDebug() << "Highlight: [" << text << "] mi:" << matchingIndex << " ml:" << matchingLength;
    QTextCursor tc = parent->textCursor();
    tc.setPosition(matchingIndex);
    tc.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, matchingLength);
    parent->setTextCursor(tc);
    parent->ensureCursorVisible();
}

void KTextEdit::Private::slotReplaceText(const QString &text, int replacementIndex, int replacedLength, int matchedLength)
{
    //qDebug() << "Replace: [" << text << "] ri:" << replacementIndex << " rl:" << replacedLength << " ml:" << matchedLength;
    QTextCursor tc = parent->textCursor();
    tc.setPosition(replacementIndex);
    tc.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, matchedLength);
    tc.removeSelectedText();
    tc.insertText(text.mid(replacementIndex, replacedLength));
    if (replace->options() & KReplaceDialog::PromptOnReplace) {
        parent->setTextCursor(tc);
        parent->ensureCursorVisible();
    }
    lastReplacedPosition = replacementIndex;
}

void KTextEdit::Private::init()
{
    KCursor::setAutoHideCursor(parent, true, false);
    parent->connect(parent, &KTextEdit::languageChanged,
                    parent, &KTextEdit::setSpellCheckingLanguage);
}

KTextDecorator::KTextDecorator(KTextEdit *textEdit):
    SpellCheckDecorator(textEdit),
    m_textEdit(textEdit)
{
}

bool KTextDecorator::isSpellCheckingEnabledForBlock(const QString &textBlock) const
{
    return m_textEdit->shouldBlockBeSpellChecked(textBlock);
}

KTextEdit::KTextEdit(const QString &text, QWidget *parent)
    : QTextEdit(text, parent), d(new Private(this))
{
    d->init();
}

KTextEdit::KTextEdit(QWidget *parent)
    : QTextEdit(parent), d(new Private(this))
{
    d->init();
}

KTextEdit::~KTextEdit()
{
    delete d;
}

const QString &KTextEdit::spellCheckingLanguage() const
{
    return d->spellCheckingLanguage;
}

void KTextEdit::setSpellCheckingLanguage(const QString &_language)
{
    if (highlighter()) {
        highlighter()->setCurrentLanguage(_language);
        highlighter()->rehighlight();
    }

    if (_language != d->spellCheckingLanguage) {
        d->spellCheckingLanguage = _language;
        emit languageChanged(_language);
    }
}

void KTextEdit::showSpellConfigDialog(const QString &windowIcon)
{
    Sonnet::ConfigDialog dialog(this);
    if (!d->spellCheckingLanguage.isEmpty()) {
        dialog.setLanguage(d->spellCheckingLanguage);
    }
    if (!windowIcon.isEmpty()) {
        dialog.setWindowIcon(QIcon::fromTheme(windowIcon, dialog.windowIcon()));
    }
    if (dialog.exec()) {
        setSpellCheckingLanguage(dialog.language());
    }
}

bool KTextEdit::event(QEvent *ev)
{
    if (ev->type() == QEvent::ShortcutOverride) {
        QKeyEvent *e = static_cast<QKeyEvent *>(ev);
        if (d->overrideShortcut(e)) {
            e->accept();
            return true;
        }
    }
    return QTextEdit::event(ev);
}

bool KTextEdit::Private::handleShortcut(const QKeyEvent *event)
{
    const int key = event->key() | event->modifiers();

    if (KStandardShortcut::copy().contains(key)) {
        parent->copy();
        return true;
    } else if (KStandardShortcut::paste().contains(key)) {
        parent->paste();
        return true;
    } else if (KStandardShortcut::cut().contains(key)) {
        parent->cut();
        return true;
    } else if (KStandardShortcut::undo().contains(key)) {
        if (!parent->isReadOnly()) {
            parent->undo();
        }
        return true;
    } else if (KStandardShortcut::redo().contains(key)) {
        if (!parent->isReadOnly()) {
            parent->redo();
        }
        return true;
    } else if (KStandardShortcut::deleteWordBack().contains(key)) {
        if (!parent->isReadOnly()) {
           parent->deleteWordBack();
        }
        return true;
    } else if (KStandardShortcut::deleteWordForward().contains(key)) {
        if (!parent->isReadOnly()) {
            parent->deleteWordForward();
        }
        return true;
    } else if (KStandardShortcut::backwardWord().contains(key)) {
        QTextCursor cursor = parent->textCursor();
        cursor.movePosition(QTextCursor::PreviousWord);
        parent->setTextCursor(cursor);
        return true;
    } else if (KStandardShortcut::forwardWord().contains(key)) {
        QTextCursor cursor = parent->textCursor();
        cursor.movePosition(QTextCursor::NextWord);
        parent->setTextCursor(cursor);
        return true;
    } else if (KStandardShortcut::next().contains(key)) {
        QTextCursor cursor = parent->textCursor();
        bool moved = false;
        qreal lastY = parent->cursorRect(cursor).bottom();
        qreal distance = 0;
        do {
            qreal y = parent->cursorRect(cursor).bottom();
            distance += qAbs(y - lastY);
            lastY = y;
            moved = cursor.movePosition(QTextCursor::Down);
        } while (moved && distance < parent->viewport()->height());

        if (moved) {
            cursor.movePosition(QTextCursor::Up);
            parent->verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepAdd);
        }
        parent->setTextCursor(cursor);
        return true;
    } else if (KStandardShortcut::prior().contains(key)) {
        QTextCursor cursor = parent->textCursor();
        bool moved = false;
        qreal lastY = parent->cursorRect(cursor).bottom();
        qreal distance = 0;
        do {
            qreal y = parent->cursorRect(cursor).bottom();
            distance += qAbs(y - lastY);
            lastY = y;
            moved = cursor.movePosition(QTextCursor::Up);
        } while (moved && distance < parent->viewport()->height());

        if (moved) {
            cursor.movePosition(QTextCursor::Down);
            parent->verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepSub);
        }
        parent->setTextCursor(cursor);
        return true;
    } else if (KStandardShortcut::begin().contains(key)) {
        QTextCursor cursor = parent->textCursor();
        cursor.movePosition(QTextCursor::Start);
        parent->setTextCursor(cursor);
        return true;
    } else if (KStandardShortcut::end().contains(key)) {
        QTextCursor cursor = parent->textCursor();
        cursor.movePosition(QTextCursor::End);
        parent->setTextCursor(cursor);
        return true;
    } else if (KStandardShortcut::beginningOfLine().contains(key)) {
        QTextCursor cursor = parent->textCursor();
        cursor.movePosition(QTextCursor::StartOfLine);
        parent->setTextCursor(cursor);
        return true;
    } else if (KStandardShortcut::endOfLine().contains(key)) {
        QTextCursor cursor = parent->textCursor();
        cursor.movePosition(QTextCursor::EndOfLine);
        parent->setTextCursor(cursor);
        return true;
    } else if (findReplaceEnabled && KStandardShortcut::find().contains(key)) {
        parent->slotFind();
        return true;
    } else if (findReplaceEnabled && KStandardShortcut::findNext().contains(key)) {
        parent->slotFindNext();
        return true;
    } else if (findReplaceEnabled && KStandardShortcut::findPrev().contains(key)) {
        parent->slotFindPrevious();
        return true;
    } else if (findReplaceEnabled && KStandardShortcut::replace().contains(key)) {
        if (!parent->isReadOnly()) {
            parent->slotReplace();
        }
        return true;
    } else if (KStandardShortcut::pasteSelection().contains(key)) {
        QString text = QApplication::clipboard()->text(QClipboard::Selection);
        if (!text.isEmpty()) {
            parent->insertPlainText(text);    // TODO: check if this is html? (MiB)
        }
        return true;
    }
    return false;
}

static void deleteWord(QTextCursor cursor, QTextCursor::MoveOperation op)
{
    cursor.clearSelection();
    cursor.movePosition(op, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
}

void KTextEdit::deleteWordBack()
{
    deleteWord(textCursor(), QTextCursor::PreviousWord);
}

void KTextEdit::deleteWordForward()
{
    deleteWord(textCursor(), QTextCursor::WordRight);
}

QMenu *KTextEdit::mousePopupMenu()
{
    QMenu *popup = createStandardContextMenu();
    if (!popup) {
        return nullptr;
    }
    connect(popup, SIGNAL(triggered(QAction*)),
            this, SLOT(menuActivated(QAction*)));

    const bool emptyDocument = document()->isEmpty();
    if (!isReadOnly()) {
        QList<QAction *> actionList = popup->actions();
        enum { UndoAct, RedoAct, CutAct, CopyAct, PasteAct, ClearAct, SelectAllAct, NCountActs };
        QAction *separatorAction = nullptr;
        int idx = actionList.indexOf(actionList[SelectAllAct]) + 1;
        if (idx < actionList.count()) {
            separatorAction = actionList.at(idx);
        }
        if (separatorAction) {
            QAction *clearAllAction = KStandardAction::clear(this, SLOT(undoableClear()), popup);
            if (emptyDocument) {
                clearAllAction->setEnabled(false);
            }
            popup->insertAction(separatorAction, clearAllAction);
        }
    }
    KIconTheme::assignIconsToContextMenu(isReadOnly() ? KIconTheme::ReadOnlyText
                                         : KIconTheme::TextEditor,
                                         popup->actions());

    if (!isReadOnly()) {
        popup->addSeparator();
        if (!d->speller) {
            d->speller = new Sonnet::Speller();
        }
        if (!d->speller->availableBackends().isEmpty()) {

            d->spellCheckAction = popup->addAction(QIcon::fromTheme(QStringLiteral("tools-check-spelling")),
                                                   i18n("Check Spelling..."));
            if (emptyDocument) {
                d->spellCheckAction->setEnabled(false);
            }
            if (checkSpellingEnabled()) {
                d->languagesMenu = new QMenu(i18n("Spell Checking Language"), popup);
                QActionGroup *languagesGroup = new QActionGroup(d->languagesMenu);
                languagesGroup->setExclusive(true);

                QMapIterator<QString, QString> i(d->speller->availableDictionaries());
                const QString language = spellCheckingLanguage();
                while (i.hasNext()) {
                    i.next();

                    QAction *languageAction = d->languagesMenu->addAction(i.key());
                    languageAction->setCheckable(true);
                    languageAction->setChecked(language == i.value() || (language.isEmpty()
                                                                         && d->speller->defaultLanguage() == i.value()));
                    languageAction->setData(i.value());
                    languageAction->setActionGroup(languagesGroup);
                    connect(languageAction, &QAction::triggered,
                            [this, languageAction]() {
                        setSpellCheckingLanguage(languageAction->data().toString());
                    });
                }
                popup->addMenu(d->languagesMenu);
            }

            d->autoSpellCheckAction = popup->addAction(i18n("Auto Spell Check"));
            d->autoSpellCheckAction->setCheckable(true);
            d->autoSpellCheckAction->setChecked(checkSpellingEnabled());
            popup->addSeparator();
        }
        if (d->showTabAction) {
            d->allowTab = popup->addAction(i18n("Allow Tabulations"));
            d->allowTab->setCheckable(true);
            d->allowTab->setChecked(!tabChangesFocus());
        }
    }

    if (d->findReplaceEnabled) {
        QAction *findAction = KStandardAction::find(this, SLOT(slotFind()), popup);
        QAction *findNextAction = KStandardAction::findNext(this, SLOT(slotFindNext()), popup);
        QAction *findPrevAction = KStandardAction::findPrev(this, SLOT(slotFindPrevious()), popup);
        if (emptyDocument) {
            findAction->setEnabled(false);
            findNextAction->setEnabled(false);
            findPrevAction->setEnabled(false);
        } else {
            findNextAction->setEnabled(d->find != nullptr);
            findPrevAction->setEnabled(d->find != nullptr);
        }
        popup->addSeparator();
        popup->addAction(findAction);
        popup->addAction(findNextAction);
        popup->addAction(findPrevAction);

        if (!isReadOnly()) {
            QAction *replaceAction = KStandardAction::replace(this, SLOT(slotReplace()), popup);
            if (emptyDocument) {
                replaceAction->setEnabled(false);
            }
            popup->addAction(replaceAction);
        }
    }
#ifdef HAVE_SPEECH
    popup->addSeparator();
    QAction *speakAction = popup->addAction(i18n("Speak Text"));
    speakAction->setIcon(QIcon::fromTheme(QStringLiteral("preferences-desktop-text-to-speech")));
    speakAction->setEnabled(!emptyDocument);
    connect(speakAction, &QAction::triggered, this, &KTextEdit::slotSpeakText);
#endif
    return popup;
}

void KTextEdit::slotSpeakText()
{
#ifdef HAVE_SPEECH
    QString text;
    if (textCursor().hasSelection()) {
        text = textCursor().selectedText();
    } else {
        text = toPlainText();
    }
    d->textToSpeech->say(text);
#endif
}

void KTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *popup = mousePopupMenu();
    if (popup) {
        aboutToShowContextMenu(popup);
        popup->exec(event->globalPos());
        delete popup;
    }
}

void KTextEdit::createHighlighter()
{
    setHighlighter(new Sonnet::Highlighter(this));
}

Sonnet::Highlighter *KTextEdit::highlighter() const
{
    if (d->decorator) {
        return d->decorator->highlighter();
    } else {
        return nullptr;
    }
}

void KTextEdit::clearDecorator()
{
    delete d->decorator;
    d->decorator = nullptr;
}

void KTextEdit::addTextDecorator(Sonnet::SpellCheckDecorator *decorator)
{
    d->decorator = decorator;
}

void KTextEdit::setHighlighter(Sonnet::Highlighter *_highLighter)
{
    KTextDecorator *decorator = new KTextDecorator(this);
    // The old default highlighter must be manually deleted.
    delete decorator->highlighter();
    decorator->setHighlighter(_highLighter);

    //KTextEdit used to take ownership of the highlighter, Sonnet::SpellCheckDecorator does not.
    //so we reparent the highlighter so it will be deleted when the decorator is destroyed
    _highLighter->setParent(decorator);
    addTextDecorator(decorator);
}

void KTextEdit::setCheckSpellingEnabled(bool check)
{
    emit checkSpellingChanged(check);
    if (check == d->spellCheckingEnabled) {
        return;
    }

    // From the above statment we know know that if we're turning checking
    // on that we need to create a new highlighter and if we're turning it
    // off we should remove the old one.

    d->spellCheckingEnabled = check;
    if (check) {
        if (hasFocus()) {
            createHighlighter();
            if (!spellCheckingLanguage().isEmpty()) {
                setSpellCheckingLanguage(spellCheckingLanguage());
            }
        }
    } else {
        clearDecorator();
    }
}

void KTextEdit::focusInEvent(QFocusEvent *event)
{
    if (d->spellCheckingEnabled && !isReadOnly() && !d->decorator) {
        createHighlighter();
    }

    QTextEdit::focusInEvent(event);
}

bool KTextEdit::checkSpellingEnabled() const
{
    return d->spellCheckingEnabled;
}

bool KTextEdit::shouldBlockBeSpellChecked(const QString &) const
{
    return true;
}

void KTextEdit::setReadOnly(bool readOnly)
{
    if (!readOnly && hasFocus() && d->spellCheckingEnabled && !d->decorator) {
        createHighlighter();
    }

    if (readOnly == isReadOnly()) {
        return;
    }

    if (readOnly) {
        delete d->decorator;
        d->decorator = nullptr;

        d->customPalette = testAttribute(Qt::WA_SetPalette);
        QPalette p = palette();
        QColor color = p.color(QPalette::Disabled, QPalette::Background);
        p.setColor(QPalette::Base, color);
        p.setColor(QPalette::Background, color);
        setPalette(p);
    } else {
        if (d->customPalette && testAttribute(Qt::WA_SetPalette)) {
            QPalette p = palette();
            QColor color = p.color(QPalette::Normal, QPalette::Base);
            p.setColor(QPalette::Base, color);
            p.setColor(QPalette::Background, color);
            setPalette(p);
        } else {
            setPalette(QPalette());
        }
    }

    QTextEdit::setReadOnly(readOnly);
}

void KTextEdit::checkSpelling()
{
    d->checkSpelling(false);
}

void KTextEdit::forceSpellChecking()
{
    d->checkSpelling(true);
}

void KTextEdit::highlightWord(int length, int pos)
{
    QTextCursor cursor(document());
    cursor.setPosition(pos);
    cursor.setPosition(pos + length, QTextCursor::KeepAnchor);
    setTextCursor(cursor);
    ensureCursorVisible();
}

void KTextEdit::replace()
{
    if (document()->isEmpty()) {  // saves having to track the text changes
        return;
    }

    if (d->repDlg) {
        KWindowSystem::activateWindow(d->repDlg->winId());
    } else {
        d->repDlg = new KReplaceDialog(this, 0,
                                       QStringList(), QStringList(), false);
        connect(d->repDlg, &KFindDialog::okClicked, this, &KTextEdit::slotDoReplace);
    }
    d->repDlg->show();
}

void KTextEdit::slotDoReplace()
{
    if (!d->repDlg) {
        // Should really assert()
        return;
    }

    if (d->repDlg->pattern().isEmpty()) {
        delete d->replace;
        d->replace = nullptr;
        ensureCursorVisible();
        return;
    }

    delete d->replace;
    d->replace = new KReplace(d->repDlg->pattern(), d->repDlg->replacement(), d->repDlg->options(), this);
    d->repIndex = 0;
    if (d->replace->options() & KFind::FromCursor || d->replace->options() & KFind::FindBackwards) {
        d->repIndex = textCursor().anchor();
    }

    // Connect highlight signal to code which handles highlighting
    // of found text.
    connect(d->replace, SIGNAL(highlight(QString,int,int)),
            this, SLOT(slotFindHighlight(QString,int,int)));
    connect(d->replace, &KFind::findNext, this, &KTextEdit::slotReplaceNext);
    connect(d->replace, SIGNAL(replace(QString,int,int,int)),
            this, SLOT(slotReplaceText(QString,int,int,int)));

    d->repDlg->close();
    slotReplaceNext();
}

void KTextEdit::slotReplaceNext()
{
    if (!d->replace) {
        return;
    }

    d->lastReplacedPosition = -1;
    if (!(d->replace->options() & KReplaceDialog::PromptOnReplace)) {
        textCursor().beginEditBlock(); // #48541
        viewport()->setUpdatesEnabled(false);
    }

    KFind::Result res = KFind::NoMatch;

    if (d->replace->needData()) {
        d->replace->setData(toPlainText(), d->repIndex);
    }
    res = d->replace->replace();
    if (!(d->replace->options() & KReplaceDialog::PromptOnReplace)) {
        textCursor().endEditBlock(); // #48541
        if (d->lastReplacedPosition >= 0) {
            QTextCursor tc = textCursor();
            tc.setPosition(d->lastReplacedPosition);
            setTextCursor(tc);
            ensureCursorVisible();
        }

        viewport()->setUpdatesEnabled(true);
        viewport()->update();
    }

    if (res == KFind::NoMatch) {
        d->replace->displayFinalDialog();
        d->replace->disconnect(this);
        d->replace->deleteLater(); // we are in a slot connected to m_replace, don't delete it right away
        d->replace = nullptr;
        ensureCursorVisible();
        //or           if ( m_replace->shouldRestart() ) { reinit (w/o FromCursor) and call slotReplaceNext(); }
    } else {
        //m_replace->closeReplaceNextDialog();
    }
}

void KTextEdit::slotDoFind()
{
    if (!d->findDlg) {
        // Should really assert()
        return;
    }
    if (d->findDlg->pattern().isEmpty()) {
        delete d->find;
        d->find = nullptr;
        return;
    }
    delete d->find;
    d->find = new KFind(d->findDlg->pattern(), d->findDlg->options(), this);
    d->findIndex = 0;
    if (d->find->options() & KFind::FromCursor || d->find->options() & KFind::FindBackwards) {
        d->findIndex = textCursor().anchor();
    }

    // Connect highlight signal to code which handles highlighting
    // of found text.
    connect(d->find, SIGNAL(highlight(QString,int,int)),
            this, SLOT(slotFindHighlight(QString,int,int)));
    connect(d->find, &KFind::findNext, this, &KTextEdit::slotFindNext);

    d->findDlg->close();
    d->find->closeFindNextDialog();
    slotFindNext();
}

void KTextEdit::slotFindNext()
{
    if (!d->find) {
        return;
    }
    if (document()->isEmpty()) {
        d->find->disconnect(this);
        d->find->deleteLater(); // we are in a slot connected to m_find, don't delete right away
        d->find = nullptr;
        return;
    }

    KFind::Result res = KFind::NoMatch;
    if (d->find->needData()) {
        d->find->setData(toPlainText(), d->findIndex);
    }
    res = d->find->find();

    if (res == KFind::NoMatch) {
        d->find->displayFinalDialog();
        d->find->disconnect(this);
        d->find->deleteLater(); // we are in a slot connected to m_find, don't delete right away
        d->find = nullptr;
        //or           if ( m_find->shouldRestart() ) { reinit (w/o FromCursor) and call slotFindNext(); }
    } else {
        //m_find->closeFindNextDialog();
    }
}

void KTextEdit::slotFindPrevious()
{
    if (!d->find) {
        return;
    }
    const long oldOptions = d->find->options();
    d->find->setOptions(oldOptions ^ KFind::FindBackwards);
    slotFindNext();
    if (d->find) {
        d->find->setOptions(oldOptions);
    }
}

void KTextEdit::slotFind()
{
    if (document()->isEmpty()) {  // saves having to track the text changes
        return;
    }

    if (d->findDlg) {
        KWindowSystem::activateWindow(d->findDlg->winId());
    } else {
        d->findDlg = new KFindDialog(this);
        connect(d->findDlg, &KFindDialog::okClicked, this, &KTextEdit::slotDoFind);
    }
    d->findDlg->show();
}

void KTextEdit::slotReplace()
{
    if (document()->isEmpty()) {  // saves having to track the text changes
        return;
    }

    if (d->repDlg) {
        KWindowSystem::activateWindow(d->repDlg->winId());
    } else {
        d->repDlg = new KReplaceDialog(this, 0,
                                       QStringList(), QStringList(), false);
        connect(d->repDlg, &KFindDialog::okClicked, this, &KTextEdit::slotDoReplace);
    }
    d->repDlg->show();
}

void KTextEdit::enableFindReplace(bool enabled)
{
    d->findReplaceEnabled = enabled;
}

void KTextEdit::showTabAction(bool show)
{
    d->showTabAction = show;
}

bool KTextEdit::Private::overrideShortcut(const QKeyEvent *event)
{
    const int key = event->key() | event->modifiers();

    if (KStandardShortcut::copy().contains(key)) {
        return true;
    } else if (KStandardShortcut::paste().contains(key)) {
        return true;
    } else if (KStandardShortcut::cut().contains(key)) {
        return true;
    } else if (KStandardShortcut::undo().contains(key)) {
        return true;
    } else if (KStandardShortcut::redo().contains(key)) {
        return true;
    } else if (KStandardShortcut::deleteWordBack().contains(key)) {
        return true;
    } else if (KStandardShortcut::deleteWordForward().contains(key)) {
        return true;
    } else if (KStandardShortcut::backwardWord().contains(key)) {
        return true;
    } else if (KStandardShortcut::forwardWord().contains(key)) {
        return true;
    } else if (KStandardShortcut::next().contains(key)) {
        return true;
    } else if (KStandardShortcut::prior().contains(key)) {
        return true;
    } else if (KStandardShortcut::begin().contains(key)) {
        return true;
    } else if (KStandardShortcut::end().contains(key)) {
        return true;
    } else if (KStandardShortcut::beginningOfLine().contains(key)) {
        return true;
    } else if (KStandardShortcut::endOfLine().contains(key)) {
        return true;
    } else if (KStandardShortcut::pasteSelection().contains(key)) {
        return true;
    } else if (findReplaceEnabled && KStandardShortcut::find().contains(key)) {
        return true;
    } else if (findReplaceEnabled && KStandardShortcut::findNext().contains(key)) {
        return true;
    } else if (findReplaceEnabled && KStandardShortcut::findPrev().contains(key)) {
        return true;
    } else if (findReplaceEnabled && KStandardShortcut::replace().contains(key)) {
        return true;
    } else if (event->matches(QKeySequence::SelectAll)) { // currently missing in QTextEdit
        return true;
    }
    return false;
}

void KTextEdit::keyPressEvent(QKeyEvent *event)
{
    if (d->handleShortcut(event)) {
        event->accept();
    } else {
        QTextEdit::keyPressEvent(event);
    }
}

void KTextEdit::showAutoCorrectButton(bool show)
{
    d->showAutoCorrectionButton = show;
}

#include "moc_ktextedit.cpp"
