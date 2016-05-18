################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/l2frame/avorion.c \
../src/l2frame/cloudvela.c \
../src/l2frame/modbus.c \
../src/l2frame/spsvirgo.c 

OBJS += \
./src/l2frame/avorion.o \
./src/l2frame/cloudvela.o \
./src/l2frame/modbus.o \
./src/l2frame/spsvirgo.o 

C_DEPS += \
./src/l2frame/avorion.d \
./src/l2frame/cloudvela.d \
./src/l2frame/modbus.d \
./src/l2frame/spsvirgo.d 


# Each subdirectory must supply rules for building sources it contributes
src/l2frame/%.o: ../src/l2frame/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	/usr/local/arm/arm-2014.05/bin/arm-none-linux-gnueabi-gcc -DTARGET_LINUX_ARM -I/usr/local/mysql_arm/include -I/usr/local/arm/linux_arm_a8/include/libxml2 -I/root/ffmpeg_build_arm/include -I/usr/local/arm/linux_arm_a8_curl/include -I/usr/local/sqlite3_arm/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


