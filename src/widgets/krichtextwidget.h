/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2008 Stephen Kelly <steveire@gmail.com>
    SPDX-FileCopyrightText: 2008 Thomas McGuire <thomas.mcguire@gmx.net>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KRICHTEXTWIDGET_H
#define KRICHTEXTWIDGET_H

#include "krichtextedit.h"

#include "ktextwidgets_export.h"

class QAction;
class KRichTextWidgetPrivate;

/*!
 * \class KRichTextWidget
 * \inmodule KTextWidgets
 *
 * \brief A KRichTextEdit with common actions.
 *
 * This class implements common actions which are often used with KRichTextEdit.
 * All you need to do is to call createActions(), and the actions will be
 * added to your KXMLGUIWindow. Remember to also add the chosen actions to
 * your application ui.rc file.
 *
 * See the KRichTextWidget::RichTextSupportValues enum for an overview of
 * supported actions.
 *
 * \image krichtextedit.png "KRichTextWidget Widget"
 *
 * \since 4.1
 */
class KTEXTWIDGETS_EXPORT KRichTextWidget : public KRichTextEdit
{
    Q_OBJECT
    Q_PROPERTY(RichTextSupport richTextSupport READ richTextSupport WRITE setRichTextSupport)
public:
    /*!
     * \enum KRichTextWidget::RichTextSupportValues
     *
     * These flags describe what actions will be created by createActions() after
     * passing a combination of these flags to setRichTextSupport().
     *
     * \value DisableRichText
     *        No rich text support at all, no actions will be created. Do
     *        not use in combination with other flags.
     * \value SupportBold
     *        Action to format the selected text as bold. If no text is
     *        selected, the word under the cursor is formatted bold.  This
     *        is a KToggleAction. The status is automatically updated when
     *        the text cursor is moved.
     * \value SupportItalic
     *        Action to format the selected text as italic. If no text is
     *        selected, the word under the cursor is formatted italic.
     *        This is a KToggleAction. The status is automatically updated
     *        when the text cursor is moved.
     * \value SupportUnderline
     *        Action to underline the selected text. If no text is
     *        selected, the word under the cursor is underlined.  This is a
     *        KToggleAction. The status is automatically updated when the
     *        text cursor is moved.
     * \value SupportStrikeOut
     *        Action to strike out the selected text. If no text is
     *        selected, the word under the cursor is struck out.  This is a
     *        KToggleAction. The status is automatically updated when the
     *        text cursor is moved.
     * \value SupportFontFamily
     *        Action to change the font family of the currently selected
     *        text. If no text is selected, the font family of the word
     *        under the cursor is changed.  Displayed as a combobox when
     *        inserted into the toolbar.  This is a KFontAction. The status
     *        is automatically updated when the text cursor is moved.
     * \value SupportFontSize
     *        Action to change the font size of the currently selected
     *        text. If no text is selected, the font size of the word under
     *        the cursor is changed.  Displayed as a combobox when inserted
     *        into the toolbar.  This is a KFontSizeAction. The status is
     *        automatically updated when the text cursor is moved.
     * \value SupportTextForegroundColor
     *        Action to change the text color of the currently selected
     *        text. If no text is selected, the text color of the word
     *        under the cursor is changed.  Opens a QColorDialog to select
     *        the color.
     * \value SupportTextBackgroundColor
     *        Action to change the background color of the currently
     *        selected text. If no text is selected, the background color
     *        of the word under the cursor is changed.  Opens a
     *        QColorDialog to select the color.
     * \value FullTextFormattingSupport
     *        A combination of all the flags above.  Includes all actions
     *        that change the format of the text.
     * \value SupportChangeListStyle
     *        Action to make the current line a list element, change the
     *        list style or remove list formatting.  Displayed as a
     *        combobox when inserted into a toolbar.  This is a
     *        KSelectAction. The status is automatically updated when the
     *        text cursor is moved.
     * \value SupportIndentLists
     *        Action to increase the current list nesting level. This makes
     *        it possible to create nested lists.
     * \value SupportDedentLists
     *        Action to decrease the current list nesting level.
     * \value FullListSupport
     *        All of the three list actions above.  Includes all
     *        list-related actions.
     * \value SupportAlignment
     *        Actions to align the current paragraph left, right, center or
     *        justify.  These actions are KToogleActions. The status is
     *        automatically updated when the text cursor is moved.
     * \value SupportRuleLine
     *        Action to insert a horizontal line.
     * \value SupportHyperlinks
     *        Action to convert the current text to a hyperlink. If no text
     *        is selected, the word under the cursor is converted.  This
     *        action opens a dialog where the user can enter the link
     *        target.
     * \value SupportFormatPainting
     *        Action to make the mouse cursor a format painter. The user
     *        can select text with that painter. The selected text gets the
     *        same format as the text that was previously selected.
     * \value SupportToPlainText
     *        Action to change the text of the whole text edit to plain
     *        text.  All rich text formatting will get lost.
     * \value SupportSuperScriptAndSubScript
     *        Actions to format text as superscript or subscript. If no
     *        text is selected, the word under the cursor is formatted as
     *        selected.  This is a KToggleAction. The status is
     *        automatically updated when the text cursor is moved.
     * \value SupportDirection
     *        Action to change direction of text to Right-To-Left or
     *        Left-To-Right.
     * \value [since 5.70] SupportHeading
     *        Action to make the current line a heading (up to six levels,
     *        corresponding to HTML h1...h6 tags).  Displayed as a combobox
     *        when inserted into a toolbar.  This is a KSelectAction. The
     *        status is automatically updated when the text cursor is
     *        moved.
     * \value FullSupport
     *        Includes all above actions for full rich text support
     */
    enum RichTextSupportValues {
        DisableRichText = 0x00,
        SupportBold = 0x01,
        SupportItalic = 0x02,
        SupportUnderline = 0x04,
        SupportStrikeOut = 0x08,
        SupportFontFamily = 0x10,
        SupportFontSize = 0x20,
        SupportTextForegroundColor = 0x40,
        SupportTextBackgroundColor = 0x80,
        FullTextFormattingSupport = 0xff,
        SupportChangeListStyle = 0x100,
        SupportIndentLists = 0x200,
        SupportDedentLists = 0x400,
        FullListSupport = 0xf00,

        // Not implemented yet.
        //         SupportCreateTables = 0x1000,
        //         SupportChangeCellMargin = 0x2000,
        //         SupportChangeCellPadding = 0x4000,
        //         SupportChangeTableBorderWidth = 0x8000,
        //         SupportChangeTableBorderColor = 0x10000,
        //         SupportChangeTableBorderStyle = 0x20000,
        //         SupportChangeCellBackground = 0x40000,
        //         SupportCellFillPatterns = 0x80000,
        //
        //         FullTableSupport = 0xff000,

        SupportAlignment = 0x100000,

        // Not yet implemented SupportImages = 0x200000,

        SupportRuleLine = 0x400000,
        SupportHyperlinks = 0x800000,
        SupportFormatPainting = 0x1000000,
        SupportToPlainText = 0x2000000,
        SupportSuperScriptAndSubScript = 0x4000000,

        // SupportChangeParagraphSpacing = 0x200000,

        SupportDirection = 0x8000000,
        SupportHeading = 0x10000000,
        FullSupport = 0xffffffff,
    };

    /*
     * Stores a combination of #RichTextSupportValues values.
     */
    Q_DECLARE_FLAGS(RichTextSupport, RichTextSupportValues)
    Q_FLAG(RichTextSupport)

    /*!
     * \brief Constructor
     * \a parent the parent widget
     */
    explicit KRichTextWidget(QWidget *parent);

    /*!
     * Constructs a KRichTextWidget object
     *
     * \a text The initial text of the text edit, which is interpreted as HTML.
     *
     * \a parent The parent widget
     */
    explicit KRichTextWidget(const QString &text, QWidget *parent = nullptr);

    /*!
     * \brief Destructor
     */
    ~KRichTextWidget() override;

    /*!
     * \brief Creates the actions and adds them to the given action collection.
     *
     * Call this before calling setupGUI() in your application, but after
     * calling setRichTextSupport().
     *
     * The XML file of your KXmlGuiWindow needs to have the action names in
     * them, so that the actions actually appear in the menu and in the toolbars.
     *
     * Below is a list of actions that are created, depending on the supported rich text
     * subset set by setRichTextSupport(). The list contains action names.
     * Those names need to be the same in your XML file.
     *
     * See the KRichTextWidget::RichTextSupportValues enum documentation for a
     * detailed explanation of each action.
     *
     * \table
     * \header
     *    \li XML Name
     *    \li RichTextSupportValues flag
     * \row
     *    \li format_text_foreground_color
     *    \li SupportTextForegroundColor
     * \row
     *    \li format_text_background_color
     *    \li SupportTextBackgroundColor
     * \row
     *    \li format_font_family
     *    \li SupportFontFamily
     * \row
     *    \li format_font_size
     *    \li SupportFontSize
     * \row
     *    \li format_text_bold
     *    \li SupportBold
     * \row
     *    \li format_text_italic
     *    \li SupportItalic
     * \row
     *    \li format_text_underline
     *    \li SupportUnderline
     * \row
     *    \li format_text_strikeout
     *    \li SupportStrikeOut
     * \row
     *    \li format_align_left
     *    \li SupportAlignment
     * \row
     *    \li format_align_center
     *    \li SupportAlignment
     * \row
     *    \li format_align_right
     *    \li SupportAlignment
     * \row
     *    \li format_align_justify
     *    \li SupportAlignment
     * \row
     *    \li direction_ltr
     *    \li SupportDirection
     * \row
     *    \li direction_rtl
     *    \li SupportDirection
     * \row
     *    \li format_list_style
     *    \li SupportChangeListStyle
     * \row
     *    \li format_list_indent_more
     *    \li SupportIndentLists
     * \row
     *    \li format_list_indent_less
     *    \li SupportDedentLists
     * \row
     *    \li insert_horizontal_rule
     *    \li SupportRuleLine
     * \row
     *    \li manage_link
     *    \li SupportHyperlinks
     * \row
     *    \li format_painter
     *    \li SupportFormatPainting
     * \row
     *    \li action_to_plain_text
     *    \li SupportToPlainText
     * \row
     *    \li format_text_subscript & format_text_superscript
     *    \li SupportSuperScriptAndSubScript
     * \row
     *    \li format_heading_level
     *    \li SupportHeading
     * \endtable
     *
     * \since 5.0
     */
    virtual QList<QAction *> createActions();

    /*!
     * \brief Sets the supported rich text subset available.
     *
     * The default is KRichTextWidget::FullSupport and will be set in the
     * constructor.
     *
     * You need to call createActions() afterwards.
     *
     * \a support The supported subset.
     */
    void setRichTextSupport(const KRichTextWidget::RichTextSupport &support);

    /*!
     * \brief Returns the supported rich text subset available.
     * Returns The supported subset.
     */
    RichTextSupport richTextSupport() const;

    /*!
     * Tells KRichTextWidget to update the state of the actions created by
     * createActions().
     *
     * This is normally automatically done, but there might be a few cases where
     * you'll need to manually call this function.
     *
     * Call this function only after calling createActions().
     */
    void updateActionStates();

public Q_SLOTS:

    /*!
     * Disables or enables all of the actions created by
     * createActions().
     *
     * This may be useful in cases where rich text mode may be set on or off.
     *
     * \a enabled Whether to enable or disable the actions.
     */
    void setActionsEnabled(bool enabled);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    Q_DECLARE_PRIVATE(KRichTextWidget)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KRichTextWidget::RichTextSupport)

#endif
