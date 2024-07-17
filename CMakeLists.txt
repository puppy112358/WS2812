
cmake_minimum_required(VERSION 3.15)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv)

if (0)
    set(CMAKE_C_COMPILER riscv-none-elf-gcc)
    set(CMAKE_CXX_COMPILER riscv-none-elf-g++)
    set(CMAKE_ASM_COMPILER riscv-none-elf-gcc)
    set(CMAKE_LINKER riscv-none-elf-ld)
    set(CMAKE_AR riscv-none-elf-ar)
    set(CMAKE_OBJCOPY riscv-none-elf-objcopy)
    set(CMAKE_OBJDUMP riscv-none-elf-objdump)
    set(SIZE riscv-none-elf-size)
    set(GDB riscv-none-elf-gdb)
else()
    set(CMAKE_C_COMPILER riscv-none-embed-gcc)
    set(CMAKE_CXX_COMPILER riscv-none-embed-g++)
    set(CMAKE_ASM_COMPILER riscv-none-embed-gcc)
    set(CMAKE_LINKER riscv-none-embed-ld)
    set(CMAKE_AR riscv-none-embed-ar)
    set(CMAKE_OBJCOPY riscv-none-embed-objcopy)
    set(CMAKE_OBJDUMP riscv-none-embed-objdump)
    set(SIZE riscv-none-embed-size)
    set(GDB riscv-none-embed-gdb)
endif ()
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


# 设置编译目标变量
# set(PRJ_TARGET "wch_template")
# 设置项目名称
project(L103pro LANGUAGES C CXX ASM)
# 设置项目根目录路径变量
# set(PRJ_DIR "C:/UserTemp/wch_cmake_template")

# add_executable(${PRJ_TARGET})
# set_target_properties(${PRJ_TARGET} PROPERTIES LINKER_LANGUAGE C)

# target_sources(${PRJ_TARGET} PUBLIC
#         "${PRJ_DIR}/User/main.c")

# MCU设置
add_compile_options(-march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8)

# 优化设置
# add_compile_options(-Os -g3)

# 标准设置
# add_compile_options(-std=gnu99)

# 编译设置        -Wunused
add_compile_options(-msave-restore -fmessage-length=0
        -fsigned-char -ffunction-sections -fdata-sections
        -fno-common -Wunused -Wuninitialized -lprintf -g -std=gnu99)


# 处理ASM文件
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
else ()
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-Og -g)
endif ()


include_directories(
        "Peripheral/inc"
        "Core"
        "Debug"
        "User")

file(GLOB_RECURSE SOURCES
        "User/*.c"
        "Startup/startup_ch32l103.S"
        "Core/*.c"
        "Debug/*.c"
        "Peripheral/src/*.c")

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/Ld/link.ld)
# 链接设置
add_link_options(-march=rv32imac -mabi=ilp32)
add_link_options(-ffunction-sections -fdata-sections -fmessage-length=0 -fstack-usage)
#add_link_options(-std=gnu11)
add_link_options(-Wall -Wfatal-errors)
add_link_options(-Wl,-check-sections -Wl,-fatal-warnings -Wl,--gc-sections -Wl,--print-memory-usage)
add_link_options(-T ${LINKER_SCRIPT})
add_link_options(-nostartfiles -Xlinker --gc-sections)
add_link_options("-Wl,-Map,${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map" -Wl,--cref)
add_link_options(--specs=nano.specs --specs=nosys.specs  -u_printf_float -u_scanf_float)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
set(LIST_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.list)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${SIZE} --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
        COMMAND ${CMAKE_OBJDUMP} --all-headers --demangle --disassemble $<TARGET_FILE:${PROJECT_NAME}.elf> > ${LIST_FILE}
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")

set(COMMON_FLAGS "-specs=nosys.specs -specs=nano.specs -u _printf_float -u _scanf_float")

