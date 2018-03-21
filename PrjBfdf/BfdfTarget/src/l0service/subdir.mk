################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/hitpony/workspace/hcu/hcusrc/l0service/configxml.c \
/home/hitpony/workspace/hcu/hcusrc/l0service/mmc.c \
/home/hitpony/workspace/hcu/hcusrc/l0service/mqtt.c \
/home/hitpony/workspace/hcu/hcusrc/l0service/sysinfo.c \
/home/hitpony/workspace/hcu/hcusrc/l0service/timer.c \
/home/hitpony/workspace/hcu/hcusrc/l0service/trace.c 

OBJS += \
./src/l0service/configxml.o \
./src/l0service/mmc.o \
./src/l0service/mqtt.o \
./src/l0service/sysinfo.o \
./src/l0service/timer.o \
./src/l0service/trace.o 

C_DEPS += \
./src/l0service/configxml.d \
./src/l0service/mmc.d \
./src/l0service/mqtt.d \
./src/l0service/sysinfo.d \
./src/l0service/timer.d \
./src/l0service/trace.d 


# Each subdirectory must supply rules for building sources it contributes
src/l0service/configxml.o: /home/hitpony/workspace/hcu/hcusrc/l0service/configxml.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -I/usr/include/curl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0service/mmc.o: /home/hitpony/workspace/hcu/hcusrc/l0service/mmc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -I/usr/include/curl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0service/mqtt.o: /home/hitpony/workspace/hcu/hcusrc/l0service/mqtt.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -I/usr/include/curl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0service/sysinfo.o: /home/hitpony/workspace/hcu/hcusrc/l0service/sysinfo.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -I/usr/include/curl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0service/timer.o: /home/hitpony/workspace/hcu/hcusrc/l0service/timer.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -I/usr/include/curl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0service/trace.o: /home/hitpony/workspace/hcu/hcusrc/l0service/trace.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -I/usr/include/curl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


