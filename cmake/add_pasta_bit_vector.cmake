include_guard(GLOBAL)
include(FetchContent)

function(add_pasta_bit_vector)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_FLAGS -includecstdint)

    FetchContent_Declare(
            pasta_bit_vector
            QUIET
            GIT_REPOSITORY git@github.com:pasta-toolbox/bit_vector
            GIT_TAG v1.0.1
    )

    FetchContent_MakeAvailable(pasta_bit_vector)

endfunction()

add_pasta_bit_vector()
