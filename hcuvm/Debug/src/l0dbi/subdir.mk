################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/l0dbi/dbicom.c 

OBJS += \
./src/l0dbi/dbicom.o 

C_DEPS += \
./src/l0dbi/dbicom.d 


# Each subdirectory must supply rules for building sources it contributes
src/l0dbi/%.o: ../src/l0dbi/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


