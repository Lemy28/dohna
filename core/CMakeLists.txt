file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

add_library(core STATIC ${srcs})

target_include_directories(core PUBLIC include)



#core项目依赖tools项目,则链接他，tools项目中的头文件路径会被传染给core
#这是由于public具有传染性
target_link_libraries(core PUBLIC tools)
target_link_libraries(core PUBLIC net)
