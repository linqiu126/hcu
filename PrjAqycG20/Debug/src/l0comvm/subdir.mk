################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/pi/workspace/hcu/hcusrc/l0comvm/vm_config.c \
/home/pi/workspace/hcu/hcusrc/l0comvm/vmlayer.c 

OBJS += \
./src/l0comvm/vm_config.o \
./src/l0comvm/vmlayer.o 

C_DEPS += \
./src/l0comvm/vm_config.d \
./src/l0comvm/vmlayer.d 


# Each subdirectory must supply rules for building sources it contributes
src/l0comvm/vm_config.o: /home/pi/workspace/hcu/hcusrc/l0comvm/vm_config.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0comvm/vmlayer.o: /home/pi/workspace/hcu/hcusrc/l0comvm/vmlayer.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


