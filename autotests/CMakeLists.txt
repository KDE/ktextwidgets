include(ECMAddTests)

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test)

macro(ktextwidgets_unit_tests)
   foreach(_testname ${ARGN})
      ecm_add_test(${_testname}.cpp TEST_NAME ktextwidgets-${_testname} LINK_LIBRARIES Qt5::Test KF5::TextWidgets KF5::ConfigWidgets)
   endforeach()
endmacro()

ktextwidgets_unit_tests(
  kfindtest
  kreplacetest
  krichtextedittest
  ktextedit_unittest
)
