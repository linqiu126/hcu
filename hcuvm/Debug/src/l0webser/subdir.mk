################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/l0webser/ftp.c \
../src/l0webser/get_time.c \
../src/l0webser/http_session.c \
../src/l0webser/init_socket.c \
../src/l0webser/webserver.c 

OBJS += \
./src/l0webser/ftp.o \
./src/l0webser/get_time.o \
./src/l0webser/http_session.o \
./src/l0webser/init_socket.o \
./src/l0webser/webserver.o 

C_DEPS += \
./src/l0webser/ftp.d \
./src/l0webser/get_time.d \
./src/l0webser/http_session.d \
./src/l0webser/init_socket.d \
./src/l0webser/webserver.d 


# Each subdirectory must supply rules for building sources it contributes
src/l0webser/%.o: ../src/l0webser/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


