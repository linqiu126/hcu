################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/l0dbi/dbiairprs.c \
../src/l0dbi/dbialcohol.c \
../src/l0dbi/dbico1.c \
../src/l0dbi/dbicom.c \
../src/l0dbi/dbiemc.c \
../src/l0dbi/dbihcho.c \
../src/l0dbi/dbihsmmp.c \
../src/l0dbi/dbihumid.c \
../src/l0dbi/dbiigm.c \
../src/l0dbi/dbiihm.c \
../src/l0dbi/dbiipm.c \
../src/l0dbi/dbiiwm.c \
../src/l0dbi/dbilightstr.c \
../src/l0dbi/dbinbiot.c \
../src/l0dbi/dbinoise.c \
../src/l0dbi/dbipm25.c \
../src/l0dbi/dbisysperfm.c \
../src/l0dbi/dbitemp.c \
../src/l0dbi/dbitoxicgas.c \
../src/l0dbi/dbiwinddir.c \
../src/l0dbi/dbiwindspd.c 

OBJS += \
./src/l0dbi/dbiairprs.o \
./src/l0dbi/dbialcohol.o \
./src/l0dbi/dbico1.o \
./src/l0dbi/dbicom.o \
./src/l0dbi/dbiemc.o \
./src/l0dbi/dbihcho.o \
./src/l0dbi/dbihsmmp.o \
./src/l0dbi/dbihumid.o \
./src/l0dbi/dbiigm.o \
./src/l0dbi/dbiihm.o \
./src/l0dbi/dbiipm.o \
./src/l0dbi/dbiiwm.o \
./src/l0dbi/dbilightstr.o \
./src/l0dbi/dbinbiot.o \
./src/l0dbi/dbinoise.o \
./src/l0dbi/dbipm25.o \
./src/l0dbi/dbisysperfm.o \
./src/l0dbi/dbitemp.o \
./src/l0dbi/dbitoxicgas.o \
./src/l0dbi/dbiwinddir.o \
./src/l0dbi/dbiwindspd.o 

C_DEPS += \
./src/l0dbi/dbiairprs.d \
./src/l0dbi/dbialcohol.d \
./src/l0dbi/dbico1.d \
./src/l0dbi/dbicom.d \
./src/l0dbi/dbiemc.d \
./src/l0dbi/dbihcho.d \
./src/l0dbi/dbihsmmp.d \
./src/l0dbi/dbihumid.d \
./src/l0dbi/dbiigm.d \
./src/l0dbi/dbiihm.d \
./src/l0dbi/dbiipm.d \
./src/l0dbi/dbiiwm.d \
./src/l0dbi/dbilightstr.d \
./src/l0dbi/dbinbiot.d \
./src/l0dbi/dbinoise.d \
./src/l0dbi/dbipm25.d \
./src/l0dbi/dbisysperfm.d \
./src/l0dbi/dbitemp.d \
./src/l0dbi/dbitoxicgas.d \
./src/l0dbi/dbiwinddir.d \
./src/l0dbi/dbiwindspd.d 


# Each subdirectory must supply rules for building sources it contributes
src/l0dbi/%.o: ../src/l0dbi/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -I/usr/include/libxml2 -I/usr/include/curl -I/root/ffmpeg_build/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


