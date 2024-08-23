set(src_dir ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB_RECURSE src_sources CONFIGURE_DEPENDS ${src_dir}/*.cpp)
add_library(src ${src_sources})
target_include_directories(src PUBLIC ${src_dir})
