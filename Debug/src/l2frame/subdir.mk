################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/l2frame/avorion.c \
../src/l2frame/cloudvela.c \
../src/l2frame/i2cbuslibra.c \
../src/l2frame/modbus.c \
../src/l2frame/spibusaries.c \
../src/l2frame/spsvirgo.c 

OBJS += \
./src/l2frame/avorion.o \
./src/l2frame/cloudvela.o \
./src/l2frame/i2cbuslibra.o \
./src/l2frame/modbus.o \
./src/l2frame/spibusaries.o \
./src/l2frame/spsvirgo.o 

C_DEPS += \
./src/l2frame/avorion.d \
./src/l2frame/cloudvela.d \
./src/l2frame/i2cbuslibra.d \
./src/l2frame/modbus.d \
./src/l2frame/spibusaries.d \
./src/l2frame/spsvirgo.d 


# Each subdirectory must supply rules for building sources it contributes
src/l2frame/%.o: ../src/l2frame/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/root/ffmpeg_build/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


