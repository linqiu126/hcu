################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/pi/workspace/hcu/hcusrc/l4bi/aiqiyc.c 

OBJS += \
./src/l4bi/aiqiyc.o 

C_DEPS += \
./src/l4bi/aiqiyc.d 


# Each subdirectory must supply rules for building sources it contributes
src/l4bi/aiqiyc.o: /home/pi/workspace/hcu/hcusrc/l4bi/aiqiyc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


