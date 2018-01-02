################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/src/l0service/mmc.c \
../src/src/l0service/mqtt.c \
../src/src/l0service/sysinfo.c \
../src/src/l0service/timer.c \
../src/src/l0service/trace.c 

OBJS += \
./src/src/l0service/mmc.o \
./src/src/l0service/mqtt.o \
./src/src/l0service/sysinfo.o \
./src/src/l0service/timer.o \
./src/src/l0service/trace.o 

C_DEPS += \
./src/src/l0service/mmc.d \
./src/src/l0service/mqtt.d \
./src/src/l0service/sysinfo.d \
./src/src/l0service/timer.d \
./src/src/l0service/trace.d 


# Each subdirectory must supply rules for building sources it contributes
src/src/l0service/%.o: ../src/src/l0service/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


