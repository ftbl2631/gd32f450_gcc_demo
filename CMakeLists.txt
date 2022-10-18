cmake_minimum_required(VERSION 3.5)

set(CMAKE_C_COMPILER_WORKS TRUE)

set(CMAKE_C_STANDARD 99)

#set(CMAKE_SYSTEM_NAME Generic)
#设置工程名字,PROJECTZ指令必须在版本之后，其它指令之前

#设置GCC工具链,
set(TOOLCHAIN_PREFIX arm-none-eabi-)
set(ARM_TOOLCHAIN_DIR /usr/bin) 

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)

project(LED_GD32F450 C ASM)
message(STATUS "Start to deal with the project: ${PROJECT_NAME}")
 
set(CMAKE_OBJCOPY ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}size)

#GCC编译flags
set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "Release")
set(MCU_FLAGS "-DCPU_GD32F450 -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(CMAKE_C_FLAGS "${MCU_FLAGS} -W -Wall -fno-common -ffunction-sections -fdata-sections -mthumb") # 公共设置
set(CMAKE_C_FLAGS_DEBUG "-g -O2")
#set(CMAKE_C_FLAGS_RELEASE "-O3")

#target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/startup")
include_directories(
    ${PROJECT_SOURCE_DIR}/devices
    ${PROJECT_SOURCE_DIR}/src
#    ${PROJECT_SOURCE_DIR}/drivers
    ${PROJECT_SOURCE_DIR}/CMSIS
    #${PROJECT_SOURCE_DIR}/Utilities
    
    #driver头文件部分
    ${PROJECT_SOURCE_DIR}/GD32F4xx_SDK_V3.0.0/Include
)

#set source code where to startup
#set_property(SOURCE ${PROJECT_SOURCE_DIR}/devices/MK22F51212/gcc/startup_MK22F51212.S PROPERTY LANGUAGE ASM)

#creat static library for s32_start
#add_library(startup STATIC
#    ${PROJECT_SOURCE_DIR}/startup/startup_MKS22F12.S
#	${PROJECT_SOURCE_DIR}/startup/system_MKS22F12.c
#    )



#creat library for other block
file(GLOB_RECURSE DRIVER 
    #driver部分
    # ${PROJECT_SOURCE_DIR}/drivers/common/*.c
    # ${PROJECT_SOURCE_DIR}/drivers/crc/*.c
    # ${PROJECT_SOURCE_DIR}/drivers/dspi/*.c
    # ${PROJECT_SOURCE_DIR}/drivers/flexcan/*.c
     ${PROJECT_SOURCE_DIR}/GD32F4xx_SDK_V3.0.0/Source/*.c

)

#Build a static library

add_library(drvlib_gd32f450 STATIC
   ${DRIVER}
   #{其它目录}
)

target_include_directories(drvlib_gd32f450 PRIVATE
    ${PROJECT_SOURCE_DIR}/devices
    ${PROJECT_SOURCE_DIR}/src

    #${PROJECT_SOURCE_DIR}/drivers
    ${PROJECT_SOURCE_DIR}/CMSIS
    #${PROJECT_SOURCE_DIR}/Utilities
    
    #driver头文件部分
    ${PROJECT_SOURCE_DIR}/GD32F4xx_SDK_V3.0.0/Include
)


set(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/devices/GD32F4xx_lds/gd32f450z_flash.ld)
set(CMAKE_EXE_LINKER_FLAGS
"--specs=nano.specs -specs=nosys.specs -T${LINKER_SCRIPT} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map,--cref -Wl,--gc-sections"
)

#AUX_SOURCE_DIRECTORY(. DIR_SRCS)
# 源文件：寻找目录下所有的源文件，加到执行目标
file(GLOB_RECURSE frdm_src 
    ${PROJECT_SOURCE_DIR}/src/*.c
    ${PROJECT_SOURCE_DIR}/devices/startup/startup_gd32f450.S
    ${PROJECT_SOURCE_DIR}/devices/system_gd32f4xx.c
    #${PROJECT_SOURCE_DIR}/Utilities/*.c
)

#main file
set(EXECTABLE_NAME ${PROJECT_NAME})
add_executable(${EXECTABLE_NAME}.elf  
    ${frdm_src}
    #${其它}
)

#link extra libraries
target_link_libraries(${EXECTABLE_NAME}.elf
   drvlib_gd32f450
)

target_include_directories(${EXECTABLE_NAME}.elf PRIVATE
    ${PROJECT_SOURCE_DIR}/devices
    ${PROJECT_SOURCE_DIR}/src
    #${PROJECT_SOURCE_DIR}/Utilities
    #${PROJECT_SOURCE_DIR}/bsp
    #${DRIVER}
)


set(ELF_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf)
set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

# display the size of the output binary after a build is finished
add_custom_command(TARGET ${EXECTABLE_NAME}.elf POST_BUILD COMMAND ${CMAKE_SIZE} --format=berkeley ${EXECTABLE_NAME}.elf)
add_custom_target(${EXECTABLE_NAME}.hex ALL DEPENDS ${EXECTABLE_NAME}.elf COMMAND ${CMAKE_OBJCOPY} -Oihex ${EXECTABLE_NAME}.elf ${EXECTABLE_NAME}.hex)
add_custom_target(${EXECTABLE_NAME}.bin ALL DEPENDS ${EXECTABLE_NAME}.elf COMMAND ${CMAKE_OBJCOPY} -Obinary ${EXECTABLE_NAME}.elf ${EXECTABLE_NAME}.bin)
#add_custom_command(TARGET "${PROJECT_NAME}.elf" POST_BUILD
#    COMMAND ${CMAKE_OBJCOPY} -Obinary ${ELF_FILE} $drvlib_gd32f450
#    COMMAND ${CMAKE_OBJCOPY} -Oihex  ${ELF_FILE} ${HEX_FILE}
#    COMMENT "Building ${PROJECT_NAME}.bin and ${PROJECT_NAME}.hex"

#    COMMAND ${CMAKE_COMMAND} -E copy ${HEX_FILE} "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hex"
#    COMMAND ${CMAKE_COMMAND} -E copy ${BIN_FILE} "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin"

#    COMMAND ${CMAKE_SIZE} --format=berkeley ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
#    COMMENT "Invoking: Cross ARM GNU Print Size"
#)