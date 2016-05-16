################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/l0service/configxml.c \
../src/l0service/mmc.c \
../src/l0service/sysinfo.c \
../src/l0service/timer.c \
../src/l0service/trace.c 

OBJS += \
./src/l0service/configxml.o \
./src/l0service/mmc.o \
./src/l0service/sysinfo.o \
./src/l0service/timer.o \
./src/l0service/trace.o 

C_DEPS += \
./src/l0service/configxml.d \
./src/l0service/mmc.d \
./src/l0service/sysinfo.d \
./src/l0service/timer.d \
./src/l0service/trace.d 


# Each subdirectory must supply rules for building sources it contributes
src/l0service/%.o: ../src/l0service/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_RASPBERRY_PI3B -I/usr/include/libxml2 -I/usr/include/curl -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


