################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/hitpony/workspace/hcu/hcusrc/l0main/hcu.c 

OBJS += \
./src/l0main/hcu.o 

C_DEPS += \
./src/l0main/hcu.d 


# Each subdirectory must supply rules for building sources it contributes
src/l0main/hcu.o: /home/hitpony/workspace/hcu/hcusrc/l0main/hcu.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -DHATE_TRIGGER_ENABLE -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -I/usr/include/curl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


