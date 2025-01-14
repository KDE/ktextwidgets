/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2002 Carsten Pfeiffer <pfeiffer@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KTEXTEDIT_H
#define KTEXTEDIT_H

#include "ktextwidgets_export.h"

#include <QTextEdit>
#include <memory>
#include <sonnet/highlighter.h>

namespace Sonnet
{
class SpellCheckDecorator;
}

class KTextEditPrivate;

/*!
 * \class KTextEdit
 * \inmodule KTextWidgets
 *
 * \brief A KDE'ified QTextEdit.
 *
 * This is just a little subclass of QTextEdit, implementing
 * some standard KDE features, like cursor auto-hiding, configurable
 * wheelscrolling (fast-scroll or zoom), spell checking and deleting of entire
 * words with Ctrl-Backspace or Ctrl-Delete.
 *
 * This text edit provides two ways of spell checking: background checking,
 * which will mark incorrectly spelled words red, and a spell check dialog,
 * which lets the user check and correct all incorrectly spelled words.
 *
 * Basic rule: whenever you want to use QTextEdit, use KTextEdit!
 *
 * \image ktextedit.png "KTextEdit Widget"
 */
class KTEXTWIDGETS_EXPORT KTextEdit : public QTextEdit // krazy:exclude=qclasses
{
    Q_OBJECT

    /*!
     * \property KTextEdit::checkSpellingEnabled
     */
    Q_PROPERTY(bool checkSpellingEnabled READ checkSpellingEnabled WRITE setCheckSpellingEnabled)

    /*!
     * \property KTextEdit::spellCheckingLanguage
     */
    Q_PROPERTY(QString spellCheckingLanguage READ spellCheckingLanguage WRITE setSpellCheckingLanguage)

public:
    /*!
     * Constructs a KTextEdit object. See QTextEdit::QTextEdit
     * for details.
     */
    explicit KTextEdit(const QString &text, QWidget *parent = nullptr);

    /*!
     * Constructs a KTextEdit object. See QTextEdit::QTextEdit
     * for details.
     */
    explicit KTextEdit(QWidget *parent = nullptr);

    ~KTextEdit() override;

    /*!
     * Sets the text edit control to the state of \a readOnly.
     *
     * Reimplemented to set a proper "deactivated" background color.
     */
    virtual void setReadOnly(bool readOnly);

    /*!
     * Turns background spell checking for this text edit to the value of \a check.
     * Note that spell checking is only available in read-writable KTextEdits.
     *
     * Enabling spell checking will set back the current highlighter to the one
     * returned by createHighlighter().
     *
     * \sa checkSpellingEnabled()
     * \sa setReadOnly()
     */
    virtual void setCheckSpellingEnabled(bool check);

    /*!
     * Returns true if background spell checking is enabled for this text edit.
     * Note that it even returns true if this is a read-only KTextEdit,
     * where spell checking is actually disabled.
     * By default spell checking is disabled.
     *
     * \sa setCheckSpellingEnabled()
     */
    virtual bool checkSpellingEnabled() const;

    /*!
     * Returns true if the given paragraph or \a block should be spellchecked.
     * For example, a mail client does not want to check quoted text, and
     * would return false here (by checking whether the block starts with a
     * quote sign).
     *
     * Always returns true by default.
     *
     */
    virtual bool shouldBlockBeSpellChecked(const QString &block) const;

    /*!
     * Selects the characters at the specified position. Any previous
     * selection will be lost. The cursor is moved to the first character
     * of the new selection.
     *
     * \a length The length of the selection, in number of characters
     *
     * \a pos The position of the first character of the selection
     */
    void highlightWord(int length, int pos);

    /*!
     * Allows to create a specific highlighter if reimplemented.
     *
     * This highlighter is set each time spell checking is toggled on by
     * calling setCheckSpellingEnabled(), but can later be overridden by calling
     * setHighlighter().
     *
     * \sa setHighlighter()
     * \sa highlighter()
     */
    virtual void createHighlighter();

    /*!
     * Returns the current highlighter, which is 0 if spell checking is disabled.
     * The default highlighter is the one created by createHighlighter(), but
     * might be overridden by setHighlighter().
     *
     * \sa setHighlighter()
     * \sa createHighlighter()
     */
    Sonnet::Highlighter *highlighter() const;

    /*!
     * Sets a custom background spellcheck highlighter for this text edit.
     * Normally, the highlighter returned by createHighlighter() will be
     * used to detect and highlight incorrectly spelled words, but this
     * function allows to set a custom highlighter.
     *
     * This has to be called after enabling spell checking with
     * setCheckSpellingEnabled(), otherwise it has no effect.
     *
     * Ownership is transferred to the KTextEdit
     *
     * \a _highLighter is the spellcheck highlighter to use
     *
     * \sa highlighter()
     * \sa createHighlighter()
     */
    void setHighlighter(Sonnet::Highlighter *_highLighter);

    /*!
     * Returns standard KTextEdit popupMenu
     * \since 4.1
     */
    virtual QMenu *mousePopupMenu();

    /*!
     * Sets the find/replace action to the value of \a enabled.
     * \since 4.1
     */
    void enableFindReplace(bool enabled);

    /*!
     * Returns the spell checking language which was set by
     * setSpellCheckingLanguage(), the spellcheck dialog or the spellcheck
     * config dialog, or an empty string if that has never been called.
     * \since 4.2
     */
    const QString &spellCheckingLanguage() const;

    /*!
     * Sets the show tab action to the value of \a show.
     * \since 4.10
     */
    void showTabAction(bool show);

    /*!
     * Shows the auto-correct button if the value of \a show is true.
     * \since 4.10
     */
    void showAutoCorrectButton(bool show);

    /*!
     * \since 4.10
     * create a modal spellcheck dialogbox and spellCheckingFinished signal we sent when
     * we finish spell checking or spellCheckingCanceled signal when we cancel spell checking
     */
    void forceSpellChecking();

Q_SIGNALS:
    /*!
     * emit signal when we activate or not autospellchecking
     *
     * \since 4.1
     */
    void checkSpellingChanged(bool);

    /*!
     * Signal sends when spell checking is finished/stopped/completed
     * \since 4.1
     */
    void spellCheckStatus(const QString &);

    /*!
     * Emitted when the user changes the language in the spellcheck dialog
     * shown by checkSpelling() or when calling setSpellCheckingLanguage().
     *
     * \a language the new language the user selected
     * \since 4.1
     */
    void languageChanged(const QString &language);

    /*!
     * Emitted before the context \a menu is displayed.
     *
     * The signal allows you to add your own entries into the
     * the context menu that is created on demand.
     *
     * \note Do not store the pointer to the QMenu
     * provided through since it is created and deleted
     * on demand.
     *
     * \since 4.5
     */
    void aboutToShowContextMenu(QMenu *menu);

    /*!
     * This signal is emitted when the spell checker wants the user to decide
     * if \a currentWord should be replaced with \a autoCorrectWord.
     *
     * \since 4.10
     */
    void spellCheckerAutoCorrect(const QString &currentWord, const QString &autoCorrectWord);

    /*!
     * signal spellCheckingFinished is sent when we finish spell check or we click on "Terminate" button in sonnet dialogbox
     * \since 4.10
     */
    void spellCheckingFinished();

    /*!
     * signal spellCheckingCanceled is sent when we cancel spell checking.
     * \since 4.10
     */
    void spellCheckingCanceled();

public Q_SLOTS:

    /*!
     * Set the spell check \a language which will be used for highlighting spelling
     * mistakes and for the spellcheck dialog.
     * The languageChanged() signal will be emitted when the new language is
     * different from the old one.
     *
     * \since 4.1
     */
    void setSpellCheckingLanguage(const QString &language);

    /*!
     * Show a dialog to check the spelling. The spellCheckStatus() signal
     * will be emitted when the spell checking dialog is closed.
     */
    void checkSpelling();

    /*!
     * Opens a Sonnet::ConfigDialog for this text edit.
     * The spellcheck language of the config dialog is set to the current spellcheck
     * language of the textedit. If the user changes the language in that dialog,
     * the languageChanged() signal is emitted.
     *
     * \a windowIcon the icon which is used for the titlebar of the spell dialog
     *                   window. Can be empty, then no icon is set.
     *
     * \since 4.2
     */
    void showSpellConfigDialog(const QString &windowIcon = QString());

    /*!
     * Create replace dialogbox
     * \since 4.1
     */
    void replace();

    /*!
     * Add custom spell checker \a decorator
     * \since 5.11
     */
    void addTextDecorator(Sonnet::SpellCheckDecorator *decorator);

    /*!
     * \brief clearDecorator clear the spellcheckerdecorator
     * \since 5.11
     */
    void clearDecorator();

protected Q_SLOTS:
    void slotDoReplace();
    void slotReplaceNext();
    void slotDoFind();
    void slotFind();
    void slotFindNext();
    void slotFindPrevious();
    void slotReplace();
    void slotSpeakText();

protected:
    bool event(QEvent *) override;

    void keyPressEvent(QKeyEvent *) override;

    void focusInEvent(QFocusEvent *) override;

    /*!
     * Deletes a word backwards from the current cursor position,
     * if available.
     */
    virtual void deleteWordBack();

    /*!
     * Deletes a word forwards from the current cursor position,
     * if available.
     */
    virtual void deleteWordForward();

    void contextMenuEvent(QContextMenuEvent *) override;

protected:
    KTEXTWIDGETS_NO_EXPORT KTextEdit(KTextEditPrivate &dd, const QString &text, QWidget *parent);
    KTEXTWIDGETS_NO_EXPORT KTextEdit(KTextEditPrivate &dd, QWidget *parent);

protected:
    std::unique_ptr<class KTextEditPrivate> const d_ptr;

private:
    Q_DECLARE_PRIVATE(KTextEdit)
};

#endif // KTEXTEDIT_H
