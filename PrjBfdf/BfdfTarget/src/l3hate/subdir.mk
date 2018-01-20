################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/hitpony/workspace/hcu/hcusrc/l3hate/hate_bfdf.c \
/home/hitpony/workspace/hcu/hcusrc/l3hate/hate_common.c \
/home/hitpony/workspace/hcu/hcusrc/l3hate/l3hate.c 

OBJS += \
./src/l3hate/hate_bfdf.o \
./src/l3hate/hate_common.o \
./src/l3hate/l3hate.o 

C_DEPS += \
./src/l3hate/hate_bfdf.d \
./src/l3hate/hate_common.d \
./src/l3hate/l3hate.d 


# Each subdirectory must supply rules for building sources it contributes
src/l3hate/hate_bfdf.o: /home/hitpony/workspace/hcu/hcusrc/l3hate/hate_bfdf.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -I/usr/include/curl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3hate/hate_common.o: /home/hitpony/workspace/hcu/hcusrc/l3hate/hate_common.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -I/usr/include/curl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3hate/l3hate.o: /home/hitpony/workspace/hcu/hcusrc/l3hate/l3hate.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -I/usr/include/curl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


