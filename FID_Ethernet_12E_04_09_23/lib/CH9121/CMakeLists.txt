# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_CH9121_SRCS 变量
aux_source_directory(. DIR_CH9121_SRCS)

include_directories(../Config)

# 生成链接库
add_library(CH9121 ${DIR_CH9121_SRCS})
target_link_libraries( CH9121 PUBLIC pico_stdlib hardware_spi)