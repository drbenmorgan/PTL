# "Hello world" in PTL with ThreadPool
add_executable(ptl_hello ptl_hello.cc)
target_link_libraries(ptl_hello PRIVATE PTL::ptl)

# "Hello world" in PTL with Tasks
add_executable(ptl_hello_task ptl_hello_task.cc)
target_link_libraries(ptl_hello_task PRIVATE PTL::ptl)

# Copy/move data
add_executable(ptl_copy_move ptl_copy_move.cc)
target_link_libraries(ptl_copy_move PRIVATE PTL::ptl)

# Basic exec/join with TaskGroup
add_executable(ptl_exec_join ptl_exec_join.cc)
target_link_libraries(ptl_exec_join PRIVATE PTL::ptl)

add_executable(ptl_exec_join_vector ptl_exec_join_vector.cc)
target_link_libraries(ptl_exec_join_vector PRIVATE PTL::ptl)

# Other ways of structuring computation
add_executable(ptl_vector_to_vector ptl_vector_to_vector.cc)
target_link_libraries(ptl_vector_to_vector PRIVATE PTL::ptl)

add_executable(ptl_vector_subtask ptl_vector_subtask.cc)
target_link_libraries(ptl_vector_subtask PRIVATE PTL::ptl)

# Handling vectors of objects
add_executable(ptl_exec_member_function ptl_exec_member_function.cc)
target_link_libraries(ptl_exec_member_function PRIVATE PTL::ptl)

# Raw Tasks with ThreadPool
add_executable(ptl_raw_task ptl_raw_task.cc)
target_link_libraries(ptl_raw_task PRIVATE PTL::ptl)

# Raw Tasks with TaskManager
add_executable(ptl_raw_taskmanager ptl_raw_taskmanager.cc)
target_link_libraries(ptl_raw_taskmanager PRIVATE PTL::ptl)


