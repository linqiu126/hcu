################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/l0service/mmc.c \
../src/l0service/mqtt.c \
../src/l0service/sysinfo.c \
../src/l0service/timer.c \
../src/l0service/trace.c 

OBJS += \
./src/l0service/mmc.o \
./src/l0service/mqtt.o \
./src/l0service/sysinfo.o \
./src/l0service/timer.o \
./src/l0service/trace.o 

C_DEPS += \
./src/l0service/mmc.d \
./src/l0service/mqtt.d \
./src/l0service/sysinfo.d \
./src/l0service/timer.d \
./src/l0service/trace.d 


# Each subdirectory must supply rules for building sources it contributes
src/l0service/%.o: ../src/l0service/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -I/usr/include/curl -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


