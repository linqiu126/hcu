################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/pi/workspace/hcu/hcusrc/l1hwopt/adc.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/apigprs.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/ble.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/camera.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/con3g4g.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/ethernet.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/gpio.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/gps.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/i2c.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/lcd.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/led.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/microphone.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/motor.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/pwm.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/relay.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/spi.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/sps232.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/sps485.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/spsapi.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/switch.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/usbnet.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/wifi.c 

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
./src/l1hwopt/led.o \
./src/l1hwopt/microphone.o \
./src/l1hwopt/motor.o \
./src/l1hwopt/pwm.o \
./src/l1hwopt/relay.o \
./src/l1hwopt/spi.o \
./src/l1hwopt/sps232.o \
./src/l1hwopt/sps485.o \
./src/l1hwopt/spsapi.o \
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
./src/l1hwopt/led.d \
./src/l1hwopt/microphone.d \
./src/l1hwopt/motor.d \
./src/l1hwopt/pwm.d \
./src/l1hwopt/relay.d \
./src/l1hwopt/spi.d \
./src/l1hwopt/sps232.d \
./src/l1hwopt/sps485.d \
./src/l1hwopt/spsapi.d \
./src/l1hwopt/switch.d \
./src/l1hwopt/usbnet.d \
./src/l1hwopt/wifi.d 


# Each subdirectory must supply rules for building sources it contributes
src/l1hwopt/adc.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/adc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/apigprs.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/apigprs.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/ble.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/ble.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/camera.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/camera.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/con3g4g.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/con3g4g.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/ethernet.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/ethernet.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/gpio.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/gpio.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/gps.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/gps.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/i2c.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/i2c.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/lcd.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/lcd.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/led.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/led.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/microphone.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/microphone.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/motor.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/motor.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/pwm.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/pwm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/relay.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/relay.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/spi.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/spi.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/sps232.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/sps232.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/sps485.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/sps485.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/spsapi.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/spsapi.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/switch.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/switch.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/usbnet.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/usbnet.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/wifi.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/wifi.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


