################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/l3app/hsmmp.c \
../src/l3app/sensorairprs.c \
../src/l3app/sensoralcohol.c \
../src/l3app/sensorco1.c \
../src/l3app/sensoremc.c \
../src/l3app/sensorhcho.c \
../src/l3app/sensorhumid.c \
../src/l3app/sensorlightstr.c \
../src/l3app/sensornoise.c \
../src/l3app/sensorpm25.c \
../src/l3app/sensortemp.c \
../src/l3app/sensortoxicgas.c \
../src/l3app/sensorwinddir.c \
../src/l3app/sensorwindspd.c \
../src/l3app/svrcon.c \
../src/l3app/syspm.c 

OBJS += \
./src/l3app/hsmmp.o \
./src/l3app/sensorairprs.o \
./src/l3app/sensoralcohol.o \
./src/l3app/sensorco1.o \
./src/l3app/sensoremc.o \
./src/l3app/sensorhcho.o \
./src/l3app/sensorhumid.o \
./src/l3app/sensorlightstr.o \
./src/l3app/sensornoise.o \
./src/l3app/sensorpm25.o \
./src/l3app/sensortemp.o \
./src/l3app/sensortoxicgas.o \
./src/l3app/sensorwinddir.o \
./src/l3app/sensorwindspd.o \
./src/l3app/svrcon.o \
./src/l3app/syspm.o 

C_DEPS += \
./src/l3app/hsmmp.d \
./src/l3app/sensorairprs.d \
./src/l3app/sensoralcohol.d \
./src/l3app/sensorco1.d \
./src/l3app/sensoremc.d \
./src/l3app/sensorhcho.d \
./src/l3app/sensorhumid.d \
./src/l3app/sensorlightstr.d \
./src/l3app/sensornoise.d \
./src/l3app/sensorpm25.d \
./src/l3app/sensortemp.d \
./src/l3app/sensortoxicgas.d \
./src/l3app/sensorwinddir.d \
./src/l3app/sensorwindspd.d \
./src/l3app/svrcon.d \
./src/l3app/syspm.d 


# Each subdirectory must supply rules for building sources it contributes
src/l3app/%.o: ../src/l3app/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	/usr/local/arm/arm-2014.05/bin/arm-none-linux-gnueabi-gcc -DTARGET_LINUX_ARM -I/usr/local/mysql_arm/include -I/usr/local/arm/linux_arm_a8/include/libxml2 -I/root/ffmpeg_build_arm/include -I/usr/local/arm/linux_arm_a8_curl/include -I/usr/local/sqlite3_arm/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


