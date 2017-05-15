################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/pi/workspace/hcu/hcusrc/l0webser/ftp.c \
/home/pi/workspace/hcu/hcusrc/l0webser/get_time.c \
/home/pi/workspace/hcu/hcusrc/l0webser/http_session.c \
/home/pi/workspace/hcu/hcusrc/l0webser/init_socket.c \
/home/pi/workspace/hcu/hcusrc/l0webser/webserver.c 

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
src/l0webser/ftp.o: /home/pi/workspace/hcu/hcusrc/l0webser/ftp.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/root/ffmpeg_build/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0webser/get_time.o: /home/pi/workspace/hcu/hcusrc/l0webser/get_time.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/root/ffmpeg_build/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0webser/http_session.o: /home/pi/workspace/hcu/hcusrc/l0webser/http_session.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/root/ffmpeg_build/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0webser/init_socket.o: /home/pi/workspace/hcu/hcusrc/l0webser/init_socket.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/root/ffmpeg_build/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0webser/webserver.o: /home/pi/workspace/hcu/hcusrc/l0webser/webserver.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/root/ffmpeg_build/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


