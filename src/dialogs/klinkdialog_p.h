/*
    klinkdialog
    SPDX-FileCopyrightText: 2008 Stephen Kelly <steveire@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef KLINKDIALOG_H
#define KLINKDIALOG_H

#include <QDialog>

class KLinkDialogPrivate;
class QString;

/*!
    \brief Dialog to allow user to configure a hyperlink.
    \since 4.1
    \internal

    This class provides a dialog to ask the user for a link target url and
    text.

    The size of the dialog is automatically saved to and restored from the
    global KDE config file.
 */
class KLinkDialog : public QDialog
{
    Q_OBJECT
public:
    /*!
     * Create a link dialog.
     *
     * \a parent  Parent widget.
     */
    explicit KLinkDialog(QWidget *parent = nullptr);

    /*!
     * Destructor
     */
    ~KLinkDialog() override;

    /*!
     * Returns the link text shown in the dialog
     *
     * \a linkText The initial text
     */
    void setLinkText(const QString &linkText);

    /*!
     * Sets the target link url shown in the dialog
     *
     * \a linkUrl The initial link target url
     */
    void setLinkUrl(const QString &linkUrl);

    /*!
     * Returns the link text entered by the user.
     * Returns The link text
     */
    QString linkText() const;

    /*!
     * Returns the target link url entered by the user.
     * Returns The link url
     */
    QString linkUrl() const;

private Q_SLOTS:
    void slotTextChanged(const QString &);

private:
    KLinkDialogPrivate *const d;
};

#endif
