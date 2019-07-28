include(ECMAddQtDesignerPlugin)

ecm_qtdesignerplugin_widget(KRichTextEdit
    TOOLTIP "Rich Text Editor (KF5)"
    GROUP "Input (KF5)"
)
ecm_qtdesignerplugin_widget(KRichTextWidget
    TOOLTIP "Rich Text Widget (KF5)"
    GROUP "Input (KF5)"
)
ecm_qtdesignerplugin_widget(KTextEdit
    TOOLTIP "Improved QTextEdit (KF5)"
    WHATSTHIS "An improved version of the QTextEdit with mail or system browser invocation support"
    GROUP "Input (KF5)"
)
ecm_qtdesignerplugin_widget(KPluralHandlingSpinBox
    TOOLTIP "A QSpinBox with plural handling for the suffix (KF5)"
    WHATSTHIS "A QSpinBox with plural handling for the suffix"
    GROUP "Input (KF5)"
)

ecm_add_qtdesignerplugin(ktextwidgetswidgets
    NAME KTextWidgetsWidgets
    OUTPUT_NAME ktextwidgets5widgets
    WIDGETS
        KRichTextEdit
        KRichTextWidget
        KTextEdit
        KPluralHandlingSpinBox
    LINK_LIBRARIES
        KF5::TextWidgets
    INSTALL_DESTINATION "${KDE_INSTALL_QTPLUGINDIR}/designer"
    COMPONENT Devel
)
