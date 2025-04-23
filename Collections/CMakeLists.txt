
add_executable (Collections
"main.cpp" 
"binary_search_tree.h"
"deque_list.h"
"deque_vector.h"
"doubly_linked_list.h"
"hash_table.h" 
"red_black_tree.h" 
"singly_linked_list.h" 
"stack_list.h"
"stack_vector.h"
"blocking_queue.h"
"blocking_queue_container.h"
"buffer_array_queue.h"
"doubly_list_queue.h" 
"singly_list_queue.h"
"vector_queue.h"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Collections PROPERTY CXX_STANDARD 20)
endif()


