################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/l0webser/get_time.c \
../src/l0webser/http_session.c \
../src/l0webser/init_socket.c \
../src/l0webser/webserver.c 

OBJS += \
./src/l0webser/get_time.o \
./src/l0webser/http_session.o \
./src/l0webser/init_socket.o \
./src/l0webser/webserver.o 

C_DEPS += \
./src/l0webser/get_time.d \
./src/l0webser/http_session.d \
./src/l0webser/init_socket.d \
./src/l0webser/webserver.d 


# Each subdirectory must supply rules for building sources it contributes
src/l0webser/%.o: ../src/l0webser/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	/usr/local/arm/arm-2014.05/bin/arm-none-linux-gnueabi-gcc -DTARGET_LINUX_ARM -I/usr/local/mysql_arm/include -I/usr/local/arm/linux_arm_a8/include/libxml2 -I/root/ffmpeg_build_arm/include -I/usr/local/arm/linux_arm_a8_curl/include -I/usr/local/sqlite3_arm/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


