################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/hitpony/workspace/hcu/hcusrc/l1hwopt/usb_adc/usb-daq-v20.c \
/home/hitpony/workspace/hcu/hcusrc/l1hwopt/usb_adc/usb_daq_test.c 

OBJS += \
./src/l1hwopt/usb_adc/usb-daq-v20.o \
./src/l1hwopt/usb_adc/usb_daq_test.o 

C_DEPS += \
./src/l1hwopt/usb_adc/usb-daq-v20.d \
./src/l1hwopt/usb_adc/usb_daq_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/l1hwopt/usb_adc/usb-daq-v20.o: /home/hitpony/workspace/hcu/hcusrc/l1hwopt/usb_adc/usb-daq-v20.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -DHATE_TRIGGER_ENABLE -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -I/usr/include/curl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l1hwopt/usb_adc/usb_daq_test.o: /home/hitpony/workspace/hcu/hcusrc/l1hwopt/usb_adc/usb_daq_test.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -DHATE_TRIGGER_ENABLE -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -I/usr/include/curl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


