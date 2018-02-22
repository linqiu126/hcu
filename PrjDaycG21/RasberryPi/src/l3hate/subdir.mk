################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/sourai/hcu/hcusrc/l3hate/hate_common.c \
/home/sourai/hcu/hcusrc/l3hate/hate_dayc.c \
/home/sourai/hcu/hcusrc/l3hate/l3hate.c 

OBJS += \
./src/l3hate/hate_common.o \
./src/l3hate/hate_dayc.o \
./src/l3hate/l3hate.o 

C_DEPS += \
./src/l3hate/hate_common.d \
./src/l3hate/hate_dayc.d \
./src/l3hate/l3hate.d 


# Each subdirectory must supply rules for building sources it contributes
src/l3hate/hate_common.o: /home/sourai/hcu/hcusrc/l3hate/hate_common.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3hate/hate_dayc.o: /home/sourai/hcu/hcusrc/l3hate/hate_dayc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3hate/l3hate.o: /home/sourai/hcu/hcusrc/l3hate/l3hate.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


