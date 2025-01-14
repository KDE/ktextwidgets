/*
    krichtextedit.h
    SPDX-FileCopyrightText: 2007 Laurent Montel <montel@kde.org>
    SPDX-FileCopyrightText: 2008 Thomas McGuire <thomas.mcguire@gmx.net>
    SPDX-FileCopyrightText: 2008 Stephen Kelly <steveire@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef KRICHTEXTEDIT_H
#define KRICHTEXTEDIT_H

#include <ktextedit.h>

class QKeyEvent;

class KRichTextEditPrivate;

#include <ktextwidgets_export.h>

/*!
 * \class KRichTextEdit
 * \inmodule KTextWidgets
 *
 * \brief A KTextEdit that supports both rich text and plain text modes.
 *
 * The KRichTextEdit class provides a widget to edit and display rich text.
 *
 * It offers several additional rich text editing functions to KTextEdit and makes
 * them easier to access including:
 *
 * \list
 * \li Changing fonts, sizes.
 * \li Font formatting, such as bold, underline, italic, foreground and
 *     background color.
 * \li Paragraph alignment
 * \li Ability to edit and remove hyperlinks
 * \li Nested list handling
 * \li Simple actions to insert tables. TODO
 * \endlist
 *
 * The KRichTextEdit can be in two modes: Rich text mode and plain text mode.
 * Calling functions which modify the format/style of the text will automatically
 * enable the rich text mode. Rich text mode is sometimes also referred to as
 * HTML mode.
 *
 * Do not call setAcceptRichText() or acceptRichText() yourself. Instead simply
 * connect to the slots which insert the rich text, use switchToPlainText() or
 * enableRichTextMode().
 *
 * \image krichtextedit.png "KRichTextEdit Widget"
 *
 * \since 4.1
 */
class KTEXTWIDGETS_EXPORT KRichTextEdit : public KTextEdit
{
    Q_OBJECT

public:
    /*!
     * \enum KRichTextEdit::Mode
     *
     * The mode the edit widget is in.
     *
     * \value Plain
     *        Plain text mode
     * \value Rich
     *        Rich text mode
     */
    enum Mode {
        Plain,
        Rich,
    };

    /*!
     * Constructs a KRichTextEdit object
     *
     * \a text The initial text of the text edit, which is interpreted as
     *             HTML.
     *
     * \a parent The parent widget
     */
    explicit KRichTextEdit(const QString &text, QWidget *parent = nullptr);

    /*!
     * Constructs a KRichTextEdit object.
     *
     * \a parent The parent widget
     */
    explicit KRichTextEdit(QWidget *parent = nullptr);

    ~KRichTextEdit() override;

    /*!
     * This enables rich text mode. Nothing is done except changing the internal
     * mode and allowing rich text pastes.
     */
    void enableRichTextMode();

    /*!
     * Returns the current text mode
     */
    Mode textMode() const;

    /*!
     * Returns the plain text string if in plain text mode or the HTML code
     *         if in rich text mode. The text is not word-wrapped.
     */
    QString textOrHtml() const;

    /*!
     * Replaces all the content of the text edit with the given string.
     * If the string is in rich text format, the text is inserted as rich text,
     * otherwise it is inserted as plain text.
     *
     * \a text The text to insert
     */
    void setTextOrHtml(const QString &text);

    /*!
     * Returns the text of the link at the current position or an empty string
     * if the cursor is not on a link.
     *
     * \sa currentLinkUrl
     */
    QString currentLinkText() const;

    /*!
     * Returns the URL target (href) of the link at the current position or an
     * empty string if the cursor is not on a link.
     *
     * \sa currentLinkText
     */
    QString currentLinkUrl() const;

    /*!
     * If the cursor is on a link, sets the \a cursor to a selection of the
     * text of the link. If the \a cursor is not on a link, selects the current word
     * or existing selection.
     *
     * \a cursor The cursor to use to select the text.
     *
     * \sa updateLink
     */
    void selectLinkText(QTextCursor *cursor) const;

    /*!
     * Convenience function to select the link text using the active cursor.
     *
     * \sa selectLinkText
     */
    void selectLinkText() const;

    /*!
     * Replaces the current selection with a hyperlink with the link URL \a linkUrl
     * and the link text \a linkText.
     *
     * \sa selectLinkText
     * \sa currentLinkUrl
     * \sa currentLinkText
     *
     * \a linkUrl The link will get this URL as href (target)
     *
     * \a linkText The link will get this alternative text, which is the
     *                 text displayed in the text edit.
     */
    void updateLink(const QString &linkUrl, const QString &linkText);

    /*!
     * Returns true if the list item at the current position can be indented.
     *
     * \sa canDedentList
     */
    bool canIndentList() const;

    /*!
     * Returns true if the list item at the current position can be dedented.
     *
     * \sa canIndentList
     */
    bool canDedentList() const;

public Q_SLOTS:

    /*!
     * Sets the alignment of the current block to Left Aligned
     */
    void alignLeft();

    /*!
     * Sets the alignment of the current block to Centered
     */
    void alignCenter();

    /*!
     * Sets the alignment of the current block to Right Aligned
     */
    void alignRight();

    /*!
     * Sets the alignment of the current block to Justified
     */
    void alignJustify();

    /*!
     * Sets the direction of the current block to Right-To-Left
     *
     * \since 4.6
     */
    void makeRightToLeft();

    /*!
     * Sets the direction of the current block to Left-To-Right
     *
     * \since 4.6
     */
    void makeLeftToRight();

    /*!
     * Sets the list style of the current list, or creates a new list using the
     * current block. The \a _styleindex corresponds to the QTextListFormat::Style
     *
     * \a _styleIndex The list will get this style
     */
    void setListStyle(int _styleIndex);

    /*!
     * Increases the nesting level of the current block or selected blocks.
     *
     * \sa canIndentList
     */
    void indentListMore();

    /*!
     * Decreases the nesting level of the current block or selected blocks.
     *
     * \sa canDedentList
     */
    void indentListLess();

    /*!
     * Sets the current word or selection to the font family \a fontFamily
     *
     * \a fontFamily The text's font family will be changed to this one
     */
    void setFontFamily(const QString &fontFamily);

    /*!
     * Sets the current word or selection to the font size \a size
     *
     * \a size The text's font will get this size
     */
    void setFontSize(int size);

    /*!
     * Sets the current word or selection to the font \a font
     *
     * \a font the font of the text will be set to this font
     */
    void setFont(const QFont &font);

    /*!
     * Toggles the bold formatting of the current word or selection at the current
     * cursor position.
     *
     * \a bold If true, the text will be set to bold
     */
    void setTextBold(bool bold);

    /*!
     * Toggles the italic formatting of the current word or selection at the current
     * cursor position.
     *
     * \a italic If true, the text will be set to italic
     */
    void setTextItalic(bool italic);

    /*!
     * Toggles the underline formatting of the current word or selection at the current
     * cursor position.
     *
     * \a underline If true, the text will be underlined
     */
    void setTextUnderline(bool underline);

    /*!
     * Toggles the strikeout formatting of the current word or selection at the current
     * cursor position.
     *
     * \a strikeOut If true, the text will be struck out
     */
    void setTextStrikeOut(bool strikeOut);

    /*!
     * Sets the foreground color of the current word or selection to \a color.
     *
     * \a color The text will get this background color
     */
    void setTextForegroundColor(const QColor &color);

    /*!
     * Sets the background color of the current word or selection to \a color.
     *
     * \a color The text will get this foreground color
     */
    void setTextBackgroundColor(const QColor &color);

    /*!
     * Inserts a horizontal rule below the current block.
     */
    void insertHorizontalRule();

    /*!
     * This will switch the editor to plain text mode.
     * All rich text formatting will be destroyed.
     */
    void switchToPlainText();

    /*!
     * This will clean some of the bad html produced by the underlying QTextEdit
     * It walks over all lines and cleans up a bit. Should be improved to produce
     * our own Html.
     */
    QString toCleanHtml() const;

    /*!
     * Toggles the superscript formatting of the current word or selection at the current
     * cursor position.
     *
     * \a superscript If true, the text will be set to superscript
     */
    void setTextSuperScript(bool superscript);

    /*!
     * Toggles the subscript formatting of the current word or selection at the current
     * cursor position.
     *
     * \a subscript If true, the text will be set to subscript
     */
    void setTextSubScript(bool subscript);

    /*!
     * Sets the heading level of a current block or selection
     *
     * \a level Heading level (value should be between 0 and 6)
     * (0 is "normal text", 1 is the largest heading, 6 is the smallest one)
     *
     * \since 5.70
     */
    void setHeadingLevel(int level);

    /*!
     * \since 4.10
     * Because of binary compatibility constraints, insertPlainText
     * is not virtual. Therefore it must dynamically detect and call this slot.
     */
    void insertPlainTextImplementation();

Q_SIGNALS:

    /*!
     * Emitted whenever the text mode is changed.
     *
     * \a mode The new text mode
     */
    void textModeChanged(KRichTextEdit::Mode mode);

protected:
    void keyPressEvent(QKeyEvent *event) override;

protected:
    KTEXTWIDGETS_NO_EXPORT KRichTextEdit(KRichTextEditPrivate &dd, const QString &text, QWidget *parent);
    KTEXTWIDGETS_NO_EXPORT KRichTextEdit(KRichTextEditPrivate &dd, QWidget *parent);

private:
    Q_DECLARE_PRIVATE(KRichTextEdit)
};

#endif
