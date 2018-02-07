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
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


