################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/pi/workspace/hcu/hcusrc/l2codec/bhcodecxml.c \
/home/pi/workspace/hcu/hcusrc/l2codec/bhcodeczhb.c \
/home/pi/workspace/hcu/hcusrc/l2codec/huixmlcodec.c 

OBJS += \
./src/l2codec/bhcodecxml.o \
./src/l2codec/bhcodeczhb.o \
./src/l2codec/huixmlcodec.o 

C_DEPS += \
./src/l2codec/bhcodecxml.d \
./src/l2codec/bhcodeczhb.d \
./src/l2codec/huixmlcodec.d 


# Each subdirectory must supply rules for building sources it contributes
src/l2codec/bhcodecxml.o: /home/pi/workspace/hcu/hcusrc/l2codec/bhcodecxml.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2codec/bhcodeczhb.o: /home/pi/workspace/hcu/hcusrc/l2codec/bhcodeczhb.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2codec/huixmlcodec.o: /home/pi/workspace/hcu/hcusrc/l2codec/huixmlcodec.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


