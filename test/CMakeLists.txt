add_executable(test_bond test_bond.cpp)

target_compile_features(test_bond PRIVATE cxx_std_20)
target_link_libraries(test_bond PRIVATE
        PyFi
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(test_bond TEST_PREFIX "unit.")