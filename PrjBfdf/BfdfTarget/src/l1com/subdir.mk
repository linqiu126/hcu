################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/hitpony/workspace/hcu/hcusrc/l1com/hwinv.c 

OBJS += \
./src/l1com/hwinv.o 

C_DEPS += \
./src/l1com/hwinv.d 


# Each subdirectory must supply rules for building sources it contributes
src/l1com/hwinv.o: /home/hitpony/workspace/hcu/hcusrc/l1com/hwinv.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -DHATE_TRIGGER_ENABLE -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -I/usr/include/curl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


