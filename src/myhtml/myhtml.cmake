
include(ExternalProject)

SET(MY_HTML_ROOT ${clucene_SOURCE_DIR}/../)

message(STATUS "start to load myhtml in ${MY_HTML_ROOT} - ${CMAKE_CURRENT_BINARY_DIR}")

ExternalProject_Add(my_html, 
  SOURCE_DIR "${MY_HTML_ROOT}" 
  PREFIX myhtml 
  BUILD_COMMAND make 
  COMMAND echo "hello world" 
  BUILD_ALWAYS ON 
  CONFIGURE_COMMAND echo "hello config"  
  BUILD_IN_SOURCE 1
)