################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/hitpony/workspace/hcu/hcusrc/l2frame/avorion.c \
/home/hitpony/workspace/hcu/hcusrc/l2frame/canitfleo.c \
/home/hitpony/workspace/hcu/hcusrc/l2frame/canlib.c \
/home/hitpony/workspace/hcu/hcusrc/l2frame/cloudvela.c \
/home/hitpony/workspace/hcu/hcusrc/l2frame/i2cbuslibra.c \
/home/hitpony/workspace/hcu/hcusrc/l2frame/l2packet.c \
/home/hitpony/workspace/hcu/hcusrc/l2frame/l2usbcan.c \
/home/hitpony/workspace/hcu/hcusrc/l2frame/llczhb.c \
/home/hitpony/workspace/hcu/hcusrc/l2frame/modbus.c \
/home/hitpony/workspace/hcu/hcusrc/l2frame/nbiotcj188.c \
/home/hitpony/workspace/hcu/hcusrc/l2frame/nbiotqg376.c \
/home/hitpony/workspace/hcu/hcusrc/l2frame/spibusaries.c \
/home/hitpony/workspace/hcu/hcusrc/l2frame/spsvirgo.c 

OBJS += \
./src/l2frame/avorion.o \
./src/l2frame/canitfleo.o \
./src/l2frame/canlib.o \
./src/l2frame/cloudvela.o \
./src/l2frame/i2cbuslibra.o \
./src/l2frame/l2packet.o \
./src/l2frame/l2usbcan.o \
./src/l2frame/llczhb.o \
./src/l2frame/modbus.o \
./src/l2frame/nbiotcj188.o \
./src/l2frame/nbiotqg376.o \
./src/l2frame/spibusaries.o \
./src/l2frame/spsvirgo.o 

C_DEPS += \
./src/l2frame/avorion.d \
./src/l2frame/canitfleo.d \
./src/l2frame/canlib.d \
./src/l2frame/cloudvela.d \
./src/l2frame/i2cbuslibra.d \
./src/l2frame/l2packet.d \
./src/l2frame/l2usbcan.d \
./src/l2frame/llczhb.d \
./src/l2frame/modbus.d \
./src/l2frame/nbiotcj188.d \
./src/l2frame/nbiotqg376.d \
./src/l2frame/spibusaries.d \
./src/l2frame/spsvirgo.d 


# Each subdirectory must supply rules for building sources it contributes
src/l2frame/avorion.o: /home/hitpony/workspace/hcu/hcusrc/l2frame/avorion.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/canitfleo.o: /home/hitpony/workspace/hcu/hcusrc/l2frame/canitfleo.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/canlib.o: /home/hitpony/workspace/hcu/hcusrc/l2frame/canlib.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/cloudvela.o: /home/hitpony/workspace/hcu/hcusrc/l2frame/cloudvela.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/i2cbuslibra.o: /home/hitpony/workspace/hcu/hcusrc/l2frame/i2cbuslibra.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/l2packet.o: /home/hitpony/workspace/hcu/hcusrc/l2frame/l2packet.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/l2usbcan.o: /home/hitpony/workspace/hcu/hcusrc/l2frame/l2usbcan.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/llczhb.o: /home/hitpony/workspace/hcu/hcusrc/l2frame/llczhb.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/modbus.o: /home/hitpony/workspace/hcu/hcusrc/l2frame/modbus.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/nbiotcj188.o: /home/hitpony/workspace/hcu/hcusrc/l2frame/nbiotcj188.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/nbiotqg376.o: /home/hitpony/workspace/hcu/hcusrc/l2frame/nbiotqg376.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/spibusaries.o: /home/hitpony/workspace/hcu/hcusrc/l2frame/spibusaries.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/spsvirgo.o: /home/hitpony/workspace/hcu/hcusrc/l2frame/spsvirgo.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


