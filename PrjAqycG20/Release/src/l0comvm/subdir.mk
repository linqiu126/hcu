################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/pi/workspace/hcu/hcusrc/l0comvm/vmlayer.c 

OBJS += \
./src/l0comvm/vmlayer.o 

C_DEPS += \
./src/l0comvm/vmlayer.d 


# Each subdirectory must supply rules for building sources it contributes
src/l0comvm/vmlayer.o: /home/pi/workspace/hcu/hcusrc/l0comvm/vmlayer.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


