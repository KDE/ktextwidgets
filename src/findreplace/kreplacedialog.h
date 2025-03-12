/*
    This file is part of the KDE project
    SPDX-FileCopyrightText: 2001 S.R. Haque <srhaque@iee.org>.
    SPDX-FileCopyrightText: 2002 David Faure <david@mandrakesoft.com>
    SPDX-FileCopyrightText: 2004 Arend van Beelen jr. <arend@auton.nl>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KREPLACEDIALOG_H
#define KREPLACEDIALOG_H

#include "ktextwidgets_export.h"

#include "kfinddialog.h"

class KReplaceDialogPrivate;

/*!
 * \class KReplaceDialog
 * \inmodule KTextWidgets
 *
 * \brief A generic "replace" dialog.
 *
 * \b Detail:
 *
 * This widget inherits from KFindDialog and implements
 * the following additional functionalities:  a replacement string
 * object and an area for a user-defined widget to extend the dialog.
 *
 * \b Example:
 *
 * To use the basic replace dialog:
 *
 * \code
 * \endcode
 *
 * To use your own extensions:
 *
 * \code
 * \endcode
 *
 * \image kreplacedialog.png "KReplaceDialog Widget"
 */
class KTEXTWIDGETS_EXPORT KReplaceDialog : public KFindDialog
{
    Q_OBJECT

public:
    /*!
     * \enum KReplaceDialog::Options
     *
     * \value PromptOnReplace
     *        Should the user be prompted before the replace operation?
     *
     * \value BackReference
     *        Use a back reference in the regular expression.
     */

    enum Options {
        PromptOnReplace = 256,
        BackReference = 512,
    };

    /*!
     * Construct a replace dialog.read-only or rather select-only combo box with a
     * parent object and a name.
     *
     * \a parent The parent object of this widget
     *
     * \a options A bitfield of the Options to be enabled.
     *
     * \a findStrings A QStringList to insert in the combo box of text to find
     *
     * \a replaceStrings A QStringList to insert in the combo box of text to
     * replace with
     *
     * \a hasSelection Whether a selection exists
     */
    explicit KReplaceDialog(QWidget *parent = nullptr,
                            long options = 0,
                            const QStringList &findStrings = QStringList(),
                            const QStringList &replaceStrings = QStringList(),
                            bool hasSelection = true);

    ~KReplaceDialog() override;

    /*!
     * Provide the list of strings to be displayed as the \a history
     * of replacement strings. The history might get truncated if it is
     * too long.
     *
     * \sa replacementHistory
     */
    void setReplacementHistory(const QStringList &history);

    /*!
     * Returns the list of history items.
     *
     * \sa setReplacementHistory
     */
    QStringList replacementHistory() const;

    /*!
     * Set the options which are enabled.
     *
     * \a options The setting of the Options.
     */
    void setOptions(long options);

    /*!
     * Returns the state of the options. Disabled options may be returned in
     * an indeterminate state.
     *
     * \sa setOptions
     */
    long options() const;

    /*!
     * Returns the replacement string.
     */
    QString replacement() const;

    /*!
     * Returns an empty widget which the user may fill with additional UI
     * elements as required. The widget occupies the width of the dialog,
     * and is positioned immediately the regular expression support widgets
     * for the replacement string.
     */
    QWidget *replaceExtension() const;

protected:
    void showEvent(QShowEvent *) override;

private:
    Q_DECLARE_PRIVATE(KReplaceDialog)
};

#endif // KREPLACEDIALOG_H
