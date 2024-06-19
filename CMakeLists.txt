cmake_minimum_required(VERSION 3.28)
project(bitvector LANGUAGES CXX)

option(INCLUDE_PASTA_BIT_VECTOR "" OFF)
if (INCLUDE_PASTA_BIT_VECTOR)
    include(cmake/add_pasta_bit_vector.cmake)
endif ()

set(TARGET_NAME bitvector)
file(GLOB_RECURSE TARGET_SRC src/*)
file(GLOB_RECURSE TARGET_INC include/*)
add_executable(${TARGET_NAME} ${TARGET_SRC} ${TARGET_INC})
target_include_directories(${TARGET_NAME} PRIVATE include)
set_target_properties(${TARGET_NAME} PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON)

if (INCLUDE_PASTA_BIT_VECTOR)
    target_compile_definitions(${TARGET_NAME} PRIVATE INCLUDE_PASTA_BIT_VECTOR)
    target_link_libraries(${TARGET_NAME} PRIVATE pasta_bit_vector)
endif ()

option(BUILD_TESTING "Build Tests" OFF)
if (BUILD_TESTING)
    enable_testing()
    get_filename_component(TESTS_ABS_PATH "test" ABSOLUTE)
    file(GLOB_RECURSE TARGET_PATHS "test/test_*.cpp")
    foreach (TARGET_PATH ${TARGET_PATHS})
        get_filename_component(TARGET_NAME "${TARGET_PATH}" NAME_WE)
        foreach (LO a b c d e f g h i j k l m n o p q r s t u v w x y z)
            string(TOUPPER ${LO} UP)
            string(REPLACE ${UP} ${LO} TARGET_NAME "${TARGET_NAME}")
        endforeach ()

        add_executable(${TARGET_NAME} "${TARGET_PATH}")
        target_include_directories(${TARGET_NAME} PRIVATE include)
        set_target_properties(${TARGET_NAME} PROPERTIES
                LINKER_LANGUAGE CXX
                CXX_STANDARD 20
                CXX_STANDARD_REQUIRED ON)

        add_test(NAME "${TARGET_NAME}" COMMAND $<TARGET_FILE:${TARGET_NAME}>)
    endforeach ()
endif ()
