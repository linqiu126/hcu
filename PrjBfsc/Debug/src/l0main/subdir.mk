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
	gcc -DTARGET_LINUX_X86 -I"/home/hitpony/workspace/hcuvm" -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


