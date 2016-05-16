################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/l1hwopt/apigpio.c \
../src/l1hwopt/apigprs.c \
../src/l1hwopt/audio.c \
../src/l1hwopt/ble.c \
../src/l1hwopt/con3g4g.c \
../src/l1hwopt/ethernet.c \
../src/l1hwopt/gps.c \
../src/l1hwopt/lcd.c \
../src/l1hwopt/sps232.c \
../src/l1hwopt/sps485.c \
../src/l1hwopt/usbnet.c \
../src/l1hwopt/wifi.c 

OBJS += \
./src/l1hwopt/apigpio.o \
./src/l1hwopt/apigprs.o \
./src/l1hwopt/audio.o \
./src/l1hwopt/ble.o \
./src/l1hwopt/con3g4g.o \
./src/l1hwopt/ethernet.o \
./src/l1hwopt/gps.o \
./src/l1hwopt/lcd.o \
./src/l1hwopt/sps232.o \
./src/l1hwopt/sps485.o \
./src/l1hwopt/usbnet.o \
./src/l1hwopt/wifi.o 

C_DEPS += \
./src/l1hwopt/apigpio.d \
./src/l1hwopt/apigprs.d \
./src/l1hwopt/audio.d \
./src/l1hwopt/ble.d \
./src/l1hwopt/con3g4g.d \
./src/l1hwopt/ethernet.d \
./src/l1hwopt/gps.d \
./src/l1hwopt/lcd.d \
./src/l1hwopt/sps232.d \
./src/l1hwopt/sps485.d \
./src/l1hwopt/usbnet.d \
./src/l1hwopt/wifi.d 


# Each subdirectory must supply rules for building sources it contributes
src/l1hwopt/%.o: ../src/l1hwopt/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


