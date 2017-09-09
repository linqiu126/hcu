################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/pi/workspace/hcu/hcusrc/l0main/hcwd.c 

OBJS += \
./l0main/hcwd.o 

C_DEPS += \
./l0main/hcwd.d 


# Each subdirectory must supply rules for building sources it contributes
l0main/hcwd.o: /home/pi/workspace/hcu/hcusrc/l0main/hcwd.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"l0main/hcwd.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


