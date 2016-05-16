################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/l4bi/aiqiyc.c 

OBJS += \
./src/l4bi/aiqiyc.o 

C_DEPS += \
./src/l4bi/aiqiyc.d 


# Each subdirectory must supply rules for building sources it contributes
src/l4bi/%.o: ../src/l4bi/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	/usr/local/arm/arm-2014.05/bin/arm-none-linux-gnueabi-gcc -DTARGET_LINUX_ARM -I/usr/local/mysql_arm/include -I/usr/local/arm/linux_arm_a8/include/libxml2 -I/root/ffmpeg_build_arm/include -I/usr/local/arm/linux_arm_a8_curl/include -I/usr/local/sqlite3_arm/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


