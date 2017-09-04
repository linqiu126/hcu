################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/hitpony/workspace/hcu/hcusrc/l3uiconnect/bfscuicomm.c \
/home/hitpony/workspace/hcu/hcusrc/l3uiconnect/jsoninotify.c 

OBJS += \
./src/l3uiconnect/bfscuicomm.o \
./src/l3uiconnect/jsoninotify.o 

C_DEPS += \
./src/l3uiconnect/bfscuicomm.d \
./src/l3uiconnect/jsoninotify.d 


# Each subdirectory must supply rules for building sources it contributes
src/l3uiconnect/bfscuicomm.o: /home/hitpony/workspace/hcu/hcusrc/l3uiconnect/bfscuicomm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3uiconnect/jsoninotify.o: /home/hitpony/workspace/hcu/hcusrc/l3uiconnect/jsoninotify.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


