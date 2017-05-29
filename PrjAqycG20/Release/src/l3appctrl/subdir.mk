################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/pi/workspace/hcu/hcusrc/l3appctrl/l3aqycg20.c 

OBJS += \
./src/l3appctrl/l3aqycg20.o 

C_DEPS += \
./src/l3appctrl/l3aqycg20.d 


# Each subdirectory must supply rules for building sources it contributes
src/l3appctrl/l3aqycg20.o: /home/pi/workspace/hcu/hcusrc/l3appctrl/l3aqycg20.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


