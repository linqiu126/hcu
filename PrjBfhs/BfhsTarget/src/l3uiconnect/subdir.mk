################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/test/workspace/hcu/hcusrc/l3uiconnect/bfhsuicomm.c 

OBJS += \
./src/l3uiconnect/bfhsuicomm.o 

C_DEPS += \
./src/l3uiconnect/bfhsuicomm.d 


# Each subdirectory must supply rules for building sources it contributes
src/l3uiconnect/bfhsuicomm.o: /home/test/workspace/hcu/hcusrc/l3uiconnect/bfhsuicomm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

