add_executable(tst_Functional tst_Functional.cpp)
target_link_libraries(tst_Functional jd-util)
add_test(NAME tst_Functional COMMAND tst_Functional)
