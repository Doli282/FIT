include(ExternalProject)
find_package(Git REQUIRED)
find_package(PCAP REQUIRED)

ExternalProject_Add(
    doctest
    PREFIX ${CMAKE_BINARY_DIR}/doctest
    GIT_REPOSITORY https://github.com/onqtam/doctest.git
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)

# Expose required variable (DOCTEST_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(doctest source_dir)
set(DOCTEST_INCLUDE_DIR ${source_dir}/doctest CACHE INTERNAL "Path to include folder for doctest")

file(GLOB monLibTestsSources src/*.c src/*.cpp)

add_executable(monlib_tests ${monLibTestsSources})
add_dependencies(monlib_tests doctest)
target_include_directories(monlib_tests PUBLIC ${DOCTEST_INCLUDE_DIR})
target_link_libraries(monlib_tests monlib)
target_link_libraries(monlib_tests ${PCAP_LIBRARY})
set_target_properties(monlib_tests
    PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

file(GLOB testCaps test_data/*pcap test_data/*cap)
foreach(src ${testCaps})
    get_filename_component(src_name "${src}" NAME)
    configure_file("${src}" "test_data/${src_name}" COPYONLY)
endforeach()

add_test(NAME monlib_tests COMMAND monlib_tests)
