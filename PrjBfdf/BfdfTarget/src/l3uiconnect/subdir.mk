################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/hitpony/workspace/hcu/hcusrc/l3uiconnect/bfdfuicomm.c 

OBJS += \
./src/l3uiconnect/bfdfuicomm.o 

C_DEPS += \
./src/l3uiconnect/bfdfuicomm.d 


# Each subdirectory must supply rules for building sources it contributes
src/l3uiconnect/bfdfuicomm.o: /home/hitpony/workspace/hcu/hcusrc/l3uiconnect/bfdfuicomm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -I/usr/include/curl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


