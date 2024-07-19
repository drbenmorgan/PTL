# "Hello world" in PTL
add_executable(ptl_hello ptl_hello.cc)
target_link_libraries(ptl_hello PRIVATE PTL::ptl)
