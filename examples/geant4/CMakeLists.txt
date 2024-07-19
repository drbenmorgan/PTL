# "Hello world" in PTL with ThreadPool
add_executable(ptl_hello ptl_hello.cc)
target_link_libraries(ptl_hello PRIVATE PTL::ptl)

# "Hello world" in PTL with Tasks
add_executable(ptl_hello_task ptl_hello_task.cc)
target_link_libraries(ptl_hello_task PRIVATE PTL::ptl)
