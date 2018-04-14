################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/pi/workspace/hcu/hcusrc/l1hwopt/usb_adc/usb-daq-v20.c \
/home/pi/workspace/hcu/hcusrc/l1hwopt/usb_adc/usb_daq_test.c 

OBJS += \
./src/l1hwopt/usb_adc/usb-daq-v20.o \
./src/l1hwopt/usb_adc/usb_daq_test.o 

C_DEPS += \
./src/l1hwopt/usb_adc/usb-daq-v20.d \
./src/l1hwopt/usb_adc/usb_daq_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/l1hwopt/usb_adc/usb-daq-v20.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/usb_adc/usb-daq-v20.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/usb_adc/usb_daq_test.o: /home/pi/workspace/hcu/hcusrc/l1hwopt/usb_adc/usb_daq_test.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


