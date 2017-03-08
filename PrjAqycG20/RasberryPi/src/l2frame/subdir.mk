################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/pi/workspace/hcu188bad/hcusrc/l2frame/avorion.c \
/home/pi/workspace/hcu188bad/hcusrc/l2frame/canitfleo.c \
/home/pi/workspace/hcu188bad/hcusrc/l2frame/canlib.c \
/home/pi/workspace/hcu188bad/hcusrc/l2frame/cloudvela.c \
/home/pi/workspace/hcu188bad/hcusrc/l2frame/i2cbuslibra.c \
/home/pi/workspace/hcu188bad/hcusrc/l2frame/llczhb.c \
/home/pi/workspace/hcu188bad/hcusrc/l2frame/modbus.c \
/home/pi/workspace/hcu188bad/hcusrc/l2frame/nbiotcj188.c \
/home/pi/workspace/hcu188bad/hcusrc/l2frame/nbiotqg376.c \
/home/pi/workspace/hcu188bad/hcusrc/l2frame/spibusaries.c \
/home/pi/workspace/hcu188bad/hcusrc/l2frame/spsvirgo.c 

OBJS += \
./src/l2frame/avorion.o \
./src/l2frame/canitfleo.o \
./src/l2frame/canlib.o \
./src/l2frame/cloudvela.o \
./src/l2frame/i2cbuslibra.o \
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
./src/l2frame/llczhb.d \
./src/l2frame/modbus.d \
./src/l2frame/nbiotcj188.d \
./src/l2frame/nbiotqg376.d \
./src/l2frame/spibusaries.d \
./src/l2frame/spsvirgo.d 


# Each subdirectory must supply rules for building sources it contributes
src/l2frame/avorion.o: /home/pi/workspace/hcu188bad/hcusrc/l2frame/avorion.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/canitfleo.o: /home/pi/workspace/hcu188bad/hcusrc/l2frame/canitfleo.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/canlib.o: /home/pi/workspace/hcu188bad/hcusrc/l2frame/canlib.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/cloudvela.o: /home/pi/workspace/hcu188bad/hcusrc/l2frame/cloudvela.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/i2cbuslibra.o: /home/pi/workspace/hcu188bad/hcusrc/l2frame/i2cbuslibra.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/llczhb.o: /home/pi/workspace/hcu188bad/hcusrc/l2frame/llczhb.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/modbus.o: /home/pi/workspace/hcu188bad/hcusrc/l2frame/modbus.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/nbiotcj188.o: /home/pi/workspace/hcu188bad/hcusrc/l2frame/nbiotcj188.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/nbiotqg376.o: /home/pi/workspace/hcu188bad/hcusrc/l2frame/nbiotqg376.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/spibusaries.o: /home/pi/workspace/hcu188bad/hcusrc/l2frame/spibusaries.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/spsvirgo.o: /home/pi/workspace/hcu188bad/hcusrc/l2frame/spsvirgo.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


