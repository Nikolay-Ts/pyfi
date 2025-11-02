include(Catch)

add_executable(test_bond test_bond.cpp test_bond_prices.cpp)

target_compile_features(test_bond PRIVATE cxx_std_20)
target_link_libraries(test_bond PRIVATE PyFi Catch2::Catch2WithMain)

catch_discover_tests(test_bond TEST_PREFIX "unit.")

add_executable(test_option test_option.cpp)

target_compile_features(test_option PRIVATE cxx_std_20)
target_link_libraries(test_option PRIVATE PyFi Catch2::Catch2WithMain)

catch_discover_tests(test_option TEST_PREFIX "unit.")
