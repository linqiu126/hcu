################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/sourai/hcu/hcusrc/l2frame/avorion.c \
/home/sourai/hcu/hcusrc/l2frame/cloudvela.c \
/home/sourai/hcu/hcusrc/l2frame/i2cbuslibra.c \
/home/sourai/hcu/hcusrc/l2frame/l2packet.c \
/home/sourai/hcu/hcusrc/l2frame/llczhb.c \
/home/sourai/hcu/hcusrc/l2frame/modbus.c \
/home/sourai/hcu/hcusrc/l2frame/nbiotcj188.c \
/home/sourai/hcu/hcusrc/l2frame/nbiotqg376.c \
/home/sourai/hcu/hcusrc/l2frame/spibusaries.c \
/home/sourai/hcu/hcusrc/l2frame/spsvirgo.c 

OBJS += \
./src/l2frame/avorion.o \
./src/l2frame/cloudvela.o \
./src/l2frame/i2cbuslibra.o \
./src/l2frame/l2packet.o \
./src/l2frame/llczhb.o \
./src/l2frame/modbus.o \
./src/l2frame/nbiotcj188.o \
./src/l2frame/nbiotqg376.o \
./src/l2frame/spibusaries.o \
./src/l2frame/spsvirgo.o 

C_DEPS += \
./src/l2frame/avorion.d \
./src/l2frame/cloudvela.d \
./src/l2frame/i2cbuslibra.d \
./src/l2frame/l2packet.d \
./src/l2frame/llczhb.d \
./src/l2frame/modbus.d \
./src/l2frame/nbiotcj188.d \
./src/l2frame/nbiotqg376.d \
./src/l2frame/spibusaries.d \
./src/l2frame/spsvirgo.d 


# Each subdirectory must supply rules for building sources it contributes
src/l2frame/avorion.o: /home/sourai/hcu/hcusrc/l2frame/avorion.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/cloudvela.o: /home/sourai/hcu/hcusrc/l2frame/cloudvela.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/i2cbuslibra.o: /home/sourai/hcu/hcusrc/l2frame/i2cbuslibra.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/l2packet.o: /home/sourai/hcu/hcusrc/l2frame/l2packet.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/llczhb.o: /home/sourai/hcu/hcusrc/l2frame/llczhb.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/modbus.o: /home/sourai/hcu/hcusrc/l2frame/modbus.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/nbiotcj188.o: /home/sourai/hcu/hcusrc/l2frame/nbiotcj188.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/nbiotqg376.o: /home/sourai/hcu/hcusrc/l2frame/nbiotqg376.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/spibusaries.o: /home/sourai/hcu/hcusrc/l2frame/spibusaries.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2frame/spsvirgo.o: /home/sourai/hcu/hcusrc/l2frame/spsvirgo.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


