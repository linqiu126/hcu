################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/l1hwopt/adc.c \
../src/l1hwopt/apigprs.c \
../src/l1hwopt/ble.c \
../src/l1hwopt/camera.c \
../src/l1hwopt/con3g4g.c \
../src/l1hwopt/ethernet.c \
../src/l1hwopt/gpio.c \
../src/l1hwopt/gps.c \
../src/l1hwopt/i2c.c \
../src/l1hwopt/lcd.c \
../src/l1hwopt/microphone.c \
../src/l1hwopt/motor.c \
../src/l1hwopt/pwm.c \
../src/l1hwopt/relay.c \
../src/l1hwopt/spi.c \
../src/l1hwopt/sps232.c \
../src/l1hwopt/sps485.c \
../src/l1hwopt/switch.c \
../src/l1hwopt/usbnet.c \
../src/l1hwopt/wifi.c 

OBJS += \
./src/l1hwopt/adc.o \
./src/l1hwopt/apigprs.o \
./src/l1hwopt/ble.o \
./src/l1hwopt/camera.o \
./src/l1hwopt/con3g4g.o \
./src/l1hwopt/ethernet.o \
./src/l1hwopt/gpio.o \
./src/l1hwopt/gps.o \
./src/l1hwopt/i2c.o \
./src/l1hwopt/lcd.o \
./src/l1hwopt/microphone.o \
./src/l1hwopt/motor.o \
./src/l1hwopt/pwm.o \
./src/l1hwopt/relay.o \
./src/l1hwopt/spi.o \
./src/l1hwopt/sps232.o \
./src/l1hwopt/sps485.o \
./src/l1hwopt/switch.o \
./src/l1hwopt/usbnet.o \
./src/l1hwopt/wifi.o 

C_DEPS += \
./src/l1hwopt/adc.d \
./src/l1hwopt/apigprs.d \
./src/l1hwopt/ble.d \
./src/l1hwopt/camera.d \
./src/l1hwopt/con3g4g.d \
./src/l1hwopt/ethernet.d \
./src/l1hwopt/gpio.d \
./src/l1hwopt/gps.d \
./src/l1hwopt/i2c.d \
./src/l1hwopt/lcd.d \
./src/l1hwopt/microphone.d \
./src/l1hwopt/motor.d \
./src/l1hwopt/pwm.d \
./src/l1hwopt/relay.d \
./src/l1hwopt/spi.d \
./src/l1hwopt/sps232.d \
./src/l1hwopt/sps485.d \
./src/l1hwopt/switch.d \
./src/l1hwopt/usbnet.d \
./src/l1hwopt/wifi.d 


# Each subdirectory must supply rules for building sources it contributes
src/l1hwopt/%.o: ../src/l1hwopt/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


