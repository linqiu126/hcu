################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/src/l0webser/ftp.c \
../src/src/l0webser/get_time.c \
../src/src/l0webser/http_session.c \
../src/src/l0webser/init_socket.c \
../src/src/l0webser/webserver.c 

OBJS += \
./src/src/l0webser/ftp.o \
./src/src/l0webser/get_time.o \
./src/src/l0webser/http_session.o \
./src/src/l0webser/init_socket.o \
./src/src/l0webser/webserver.o 

C_DEPS += \
./src/src/l0webser/ftp.d \
./src/src/l0webser/get_time.d \
./src/src/l0webser/http_session.d \
./src/src/l0webser/init_socket.d \
./src/src/l0webser/webserver.d 


# Each subdirectory must supply rules for building sources it contributes
src/src/l0webser/%.o: ../src/src/l0webser/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


