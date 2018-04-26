################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/hitpony/Workspace/hcu/hcusrc/l3hate/hate_bfsc.c \
/home/hitpony/Workspace/hcu/hcusrc/l3hate/hate_common.c \
/home/hitpony/Workspace/hcu/hcusrc/l3hate/l3hate.c 

OBJS += \
./src/l3hate/hate_bfsc.o \
./src/l3hate/hate_common.o \
./src/l3hate/l3hate.o 

C_DEPS += \
./src/l3hate/hate_bfsc.d \
./src/l3hate/hate_common.d \
./src/l3hate/l3hate.d 


# Each subdirectory must supply rules for building sources it contributes
src/l3hate/hate_bfsc.o: /home/hitpony/Workspace/hcu/hcusrc/l3hate/hate_bfsc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3hate/hate_common.o: /home/hitpony/Workspace/hcu/hcusrc/l3hate/hate_common.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3hate/l3hate.o: /home/hitpony/Workspace/hcu/hcusrc/l3hate/l3hate.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0  -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


