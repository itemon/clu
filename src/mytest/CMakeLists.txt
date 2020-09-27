
PROJECT (my_test)

SET(MY_TEST_ALL_FILES ./mytest.cpp)

INCLUDE_DIRECTORIES( ${clucene_SOURCE_DIR}/src/mytest )

#find our headers
file(GLOB_RECURSE OUR_HEADERS ${my_test_SOURCE_DIR}/*.h)

add_library(my_test SHARED ${MY_TEST_ALL_FILES} ${OUR_HEADERS})

add_custom_target(my_test_print echo "hello world")