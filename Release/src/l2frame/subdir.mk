################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/l2frame/avorion.c \
../src/l2frame/cloudcont.c \
../src/l2frame/modbus.c 

OBJS += \
./src/l2frame/avorion.o \
./src/l2frame/cloudcont.o \
./src/l2frame/modbus.o 

C_DEPS += \
./src/l2frame/avorion.d \
./src/l2frame/cloudcont.d \
./src/l2frame/modbus.d 


# Each subdirectory must supply rules for building sources it contributes
src/l2frame/%.o: ../src/l2frame/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


