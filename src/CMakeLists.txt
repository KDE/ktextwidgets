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
)

add_library(KF5TextWidgets ${ktextwidgets_LIB_SRCS})
generate_export_header(KF5TextWidgets BASE_NAME KTextWidgets)
add_library(KF5::TextWidgets ALIAS KF5TextWidgets)

set(ktextwidgets_INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
  ${CMAKE_CURRENT_SOURCE_DIR}/findreplace
  ${CMAKE_CURRENT_SOURCE_DIR}/kregexpeditor
  ${CMAKE_CURRENT_SOURCE_DIR}/widgets
)
target_include_directories(KF5TextWidgets PUBLIC "$<BUILD_INTERFACE:${ktextwidgets_INCLUDES}>"
)

target_include_directories(KF5TextWidgets INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/KTextWidgets>")

target_link_libraries(KF5TextWidgets
PUBLIC
  Qt5::Widgets
  KF5::SonnetUi
PRIVATE
  Qt5::DBus
  KF5::SonnetCore
  KF5::I18n
  KF5::Service
  KF5::ConfigWidgets
  KF5::WindowSystem
  KF5::IconThemes
  KF5::Completion
)

set_target_properties(KF5TextWidgets PROPERTIES VERSION ${KTEXTWIDGETS_VERSION_STRING}
                                                SOVERSION ${KTEXTWIDGETS_SOVERSION}
                                                EXPORT_NAME TextWidgets
)

ecm_generate_headers(KTextWidgets_HEADERS
  HEADER_NAMES
  KRichTextEdit
  KRichTextWidget
  KTextEdit

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

install(TARGETS KF5TextWidgets EXPORT KF5TextWidgetsTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/ktextwidgets_export.h
  ${KTextWidgets_HEADERS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/KTextWidgets COMPONENT Devel
)
install( FILES kregexpeditor/kregexpeditor.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR} )

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KTextWidgets LIB_NAME KF5TextWidgets DEPS "widgets SonnetUi" FILENAME_VAR PRI_FILENAME)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
