# customize output directory
set(MY_OUTPUT_DIR examples)

# Introduction to C++ and Concurrency
add_subdirectory("q0_basics")
add_subdirectory("q1_threads")
add_subdirectory("q2_processes")

# Inter-Process Communication
add_subdirectory("q3_datapools")
add_subdirectory("q4_pipes")
add_subdirectory("q5_message_queues")
add_subdirectory("q18_socket")

# Synchronization
add_subdirectory("q6_mutex")
add_subdirectory("q7_semaphore")
add_subdirectory("q8_condition")
add_subdirectory("q9_condition_variable")
add_subdirectory("q10_event")
add_subdirectory("q11_timer")
add_subdirectory("q12_rendezvous")
add_subdirectory("q13_monitor")

# Advanced Synchronization Problems
add_subdirectory("q14_reader_writer")
add_subdirectory("q15_producer_consumer")
add_subdirectory("q16_sleeping_barber")
add_subdirectory("q17_dining_philosophers")