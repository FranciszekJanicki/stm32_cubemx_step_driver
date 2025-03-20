add_library(step_motor STATIC)

target_sources(step_motor PRIVATE 
    "step_motor.cpp"
)

target_include_directories(step_motor PUBLIC 
    "."
)

target_link_libraries(step_motor PRIVATE
    a4988
    utility
)

target_compile_options(step_motor PUBLIC
    -std=c++23
    -Wall
    -Wextra
    -Wconversion
    -Wshadow
    -Wpedantic
    -Wnarrowing
    -Waddress
    -pedantic
    -Wdeprecated
    -Wsign-conversion
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wnull-dereference
    -Wdouble-promotion
    -Wimplicit-fallthrough
    -Wcast-align
    -fconcepts
)