################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CFG_SRCS += \
../PotBot.cfg 

CMD_SRCS += \
../F2802x_Headers_BIOS.cmd \
../TMS320F28027.cmd 

C_SRCS += \
../F2802x_GlobalVariableDefs.c \
../adc.c \
../dev_init.c \
../dsp.c \
../main.c \
../servo.c \
../uart.c 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_FILES += \
./configPkg/linker.cmd \
./configPkg/compiler.opt 

GEN_MISC_DIRS += \
./configPkg/ 

C_DEPS += \
./F2802x_GlobalVariableDefs.d \
./adc.d \
./dev_init.d \
./dsp.d \
./main.d \
./servo.d \
./uart.d 

GEN_OPTS += \
./configPkg/compiler.opt 

OBJS += \
./F2802x_GlobalVariableDefs.obj \
./adc.obj \
./dev_init.obj \
./dsp.obj \
./main.obj \
./servo.obj \
./uart.obj 

GEN_MISC_DIRS__QUOTED += \
"configPkg\" 

OBJS__QUOTED += \
"F2802x_GlobalVariableDefs.obj" \
"adc.obj" \
"dev_init.obj" \
"dsp.obj" \
"main.obj" \
"servo.obj" \
"uart.obj" 

C_DEPS__QUOTED += \
"F2802x_GlobalVariableDefs.d" \
"adc.d" \
"dev_init.d" \
"dsp.d" \
"main.d" \
"servo.d" \
"uart.d" 

GEN_FILES__QUOTED += \
"configPkg\linker.cmd" \
"configPkg\compiler.opt" 

C_SRCS__QUOTED += \
"../F2802x_GlobalVariableDefs.c" \
"../adc.c" \
"../dev_init.c" \
"../dsp.c" \
"../main.c" \
"../servo.c" \
"../uart.c" 


