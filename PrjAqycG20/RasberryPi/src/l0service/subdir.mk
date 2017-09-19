################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/pi/workspace/hcu/hcusrc/l0service/configxml.c \
/home/pi/workspace/hcu/hcusrc/l0service/mmc.c \
/home/pi/workspace/hcu/hcusrc/l0service/mqtt.c \
/home/pi/workspace/hcu/hcusrc/l0service/sysinfo.c \
/home/pi/workspace/hcu/hcusrc/l0service/trace.c 

OBJS += \
./src/l0service/configxml.o \
./src/l0service/mmc.o \
./src/l0service/mqtt.o \
./src/l0service/sysinfo.o \
./src/l0service/trace.o 

C_DEPS += \
./src/l0service/configxml.d \
./src/l0service/mmc.d \
./src/l0service/mqtt.d \
./src/l0service/sysinfo.d \
./src/l0service/trace.d 


# Each subdirectory must supply rules for building sources it contributes
src/l0service/configxml.o: /home/pi/workspace/hcu/hcusrc/l0service/configxml.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0service/mmc.o: /home/pi/workspace/hcu/hcusrc/l0service/mmc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0service/mqtt.o: /home/pi/workspace/hcu/hcusrc/l0service/mqtt.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0service/sysinfo.o: /home/pi/workspace/hcu/hcusrc/l0service/sysinfo.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0service/trace.o: /home/pi/workspace/hcu/hcusrc/l0service/trace.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


