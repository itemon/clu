
PROJECT (my_test)
DEFINE_OPTIONS(EXTRA_OPTIONS EXTRA_LIBS)
ADD_DEFINITIONS(${EXTRA_OPTIONS})

set(CMAKE_VERBOSE_MAKEFILE on)

ADD_DEFINITIONS(-DMYTEST_INC_FINGER=on)

SET(MY_TEST_ALL_FILES ./mytest.cpp ./tool.cpp)

SET ( clucene_contrib_extra_libs clucene-contribs-lib clucene-core clucene-shared ${EXTRA_LIBS} )

INCLUDE_DIRECTORIES( ${clucene_SOURCE_DIR}/src/core )
INCLUDE_DIRECTORIES( ${clucene_SOURCE_DIR}/src/contribs-lib)
INCLUDE_DIRECTORIES( ${clucene_SOURCE_DIR}/src/mytest )

#find our headers
file(GLOB_RECURSE OUR_HEADERS ${my_test_SOURCE_DIR}/*.h)

add_library(my_test SHARED ${MY_TEST_ALL_FILES} ${OUR_HEADERS})
set_property(TARGET my_test PROPERTY C_VISIBILITY_PRESET hidden)

#set_property(TARGET my_test PROPERTY INSTALL_NAME_DIR @rpath/libmy_test.dylib)
set_property(TARGET my_test PROPERTY SKIP_BUILD_RPATH 1)
set_property(TARGET my_test PROPERTY BUILD_WITH_INSTALL_NAME_DIR 1)
set_property(TARGET my_test PROPERTY INSTALL_NAME_DIR "@rpath")

TARGET_LINK_LIBRARIES(my_test ${clucene_contrib_extra_libs})

add_custom_target(my_test_print echo "hello world")
message("build for xcode ${ENABLE_BUILD_FOR_XCODE}")

#add_custom_command(TARGET my_test POST_BUILD COMMAND BUD_DIR=${CMAKE_CURRENT_BINARY_DIR} sh ${CMAKE_CURRENT_SOURCE_DIR}/bud.sh)