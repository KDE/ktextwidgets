set(ktextwidgets_LIB_SRCS
  dialogs/klinkdialog.cpp
  findreplace/kfind.cpp
  findreplace/kfinddialog.cpp
  findreplace/kreplace.cpp
  findreplace/kreplacedialog.cpp
  widgets/krichtextedit.cpp
  widgets/krichtextwidget.cpp
  widgets/ktextedit.cpp
  widgets/nestedlisthelper.cpp
  widgets/kpluralhandlingspinbox.cpp
)

add_library(KF5TextWidgets ${ktextwidgets_LIB_SRCS})
add_library(KF5::TextWidgets ALIAS KF5TextWidgets)
ecm_generate_export_header(KF5TextWidgets
    BASE_NAME KTextWidgets
    GROUP_BASE_NAME KF
    VERSION ${KF5_VERSION}
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS 5.0
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)

set(ktextwidgets_INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
  ${CMAKE_CURRENT_SOURCE_DIR}/findreplace
  ${CMAKE_CURRENT_SOURCE_DIR}/kregexpeditor
  ${CMAKE_CURRENT_SOURCE_DIR}/widgets
)
target_include_directories(KF5TextWidgets PUBLIC "$<BUILD_INTERFACE:${ktextwidgets_INCLUDES}>"
)

target_include_directories(KF5TextWidgets INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/KTextWidgets>")

target_link_libraries(KF5TextWidgets
PUBLIC
  Qt5::Widgets
  KF5::SonnetUi
  KF5::I18n
PRIVATE
  KF5::SonnetCore
  KF5::Service
  KF5::ConfigWidgets
  KF5::WindowSystem
  KF5::IconThemes
  KF5::Completion
)

if (Qt5TextToSpeech_FOUND)
    target_link_libraries(KF5TextWidgets
PRIVATE
        Qt5::TextToSpeech)
endif()


set_target_properties(KF5TextWidgets PROPERTIES VERSION ${KTEXTWIDGETS_VERSION_STRING}
                                                SOVERSION ${KTEXTWIDGETS_SOVERSION}
                                                EXPORT_NAME TextWidgets
)

ecm_generate_headers(KTextWidgets_HEADERS
  HEADER_NAMES
  KRichTextEdit
  KRichTextWidget
  KTextEdit
  KPluralHandlingSpinBox

  RELATIVE widgets
  REQUIRED_HEADERS KTextWidgets_HEADERS
)
ecm_generate_headers(KTextWidgets_HEADERS
  HEADER_NAMES
  KFind
  KFindDialog
  KReplace
  KReplaceDialog

  RELATIVE findreplace
  REQUIRED_HEADERS KTextWidgets_HEADERS
)

ecm_generate_headers(KTextWidgets_HEADERS
  HEADER_NAMES
  KRegExpEditorInterface

  RELATIVE kregexpeditor
  REQUIRED_HEADERS KTextWidgets_HEADERS
)

install(TARGETS KF5TextWidgets EXPORT KF5TextWidgetsTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/ktextwidgets_export.h
  ${KTextWidgets_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KTextWidgets COMPONENT Devel
)
install( FILES kregexpeditor/kregexpeditor.desktop DESTINATION ${KDE_INSTALL_KSERVICETYPES5DIR} )

if (BUILD_DESIGNERPLUGIN)
    add_subdirectory(designer)
endif()

if (BUILD_QCH)
    ecm_add_qch(
        KF5TextWidgets_QCH
        NAME KTextWidgets
        BASE_NAME KF5TextWidgets
        VERSION ${KF5_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${KTextWidgets_HEADERS}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        IMAGE_DIRS "${CMAKE_SOURCE_DIR}/docs/pics"
        LINK_QCHS
            Qt5Widgets_QCH
            KF5SonnetUi_QCH
            KF5I18n_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
            ${ktextwidgets_INCLUDES}
        BLANK_MACROS
            KTEXTWIDGETS_EXPORT
            KTEXTWIDGETS_DEPRECATED_EXPORT
            KTEXTWIDGETS_DEPRECATED
            "KTEXTWIDGETS_DEPRECATED_VERSION(x, y, t)"
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KTextWidgets LIB_NAME KF5TextWidgets DEPS "widgets SonnetUi KI18n" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/KTextWidgets)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
