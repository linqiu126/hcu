################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/hitpony/workspace/hcu/hcusrc/l3appctrl/l3bfsc.c 

OBJS += \
./src/l3appctrl/l3bfsc.o 

C_DEPS += \
./src/l3appctrl/l3bfsc.d 


# Each subdirectory must supply rules for building sources it contributes
src/l3appctrl/l3bfsc.o: /home/hitpony/workspace/hcu/hcusrc/l3appctrl/l3bfsc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


