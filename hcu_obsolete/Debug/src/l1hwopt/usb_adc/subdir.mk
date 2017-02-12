################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/l1hwopt/usb_adc/usb-daq-v20.c \
../src/l1hwopt/usb_adc/usb_daq_test.c 

OBJS += \
./src/l1hwopt/usb_adc/usb-daq-v20.o \
./src/l1hwopt/usb_adc/usb_daq_test.o 

C_DEPS += \
./src/l1hwopt/usb_adc/usb-daq-v20.d \
./src/l1hwopt/usb_adc/usb_daq_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/l1hwopt/usb_adc/%.o: ../src/l1hwopt/usb_adc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/root/ffmpeg_build/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


