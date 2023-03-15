
file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

add_library(tools STATIC ${srcs})

target_include_directories(tools PUBLIC include)

