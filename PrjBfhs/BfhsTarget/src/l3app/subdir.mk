################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/test/workspace/hcu/hcusrc/l3app/hsmmp.c \
/home/test/workspace/hcu/hcusrc/l3app/sensorairprs.c \
/home/test/workspace/hcu/hcusrc/l3app/sensoralcohol.c \
/home/test/workspace/hcu/hcusrc/l3app/sensorco1.c \
/home/test/workspace/hcu/hcusrc/l3app/sensoremc.c \
/home/test/workspace/hcu/hcusrc/l3app/sensorhcho.c \
/home/test/workspace/hcu/hcusrc/l3app/sensorhumid.c \
/home/test/workspace/hcu/hcusrc/l3app/sensorigm.c \
/home/test/workspace/hcu/hcusrc/l3app/sensorihm.c \
/home/test/workspace/hcu/hcusrc/l3app/sensoripm.c \
/home/test/workspace/hcu/hcusrc/l3app/sensoriwm.c \
/home/test/workspace/hcu/hcusrc/l3app/sensorlightstr.c \
/home/test/workspace/hcu/hcusrc/l3app/sensornoise.c \
/home/test/workspace/hcu/hcusrc/l3app/sensorpm25.c \
/home/test/workspace/hcu/hcusrc/l3app/sensorpm25sharp.c \
/home/test/workspace/hcu/hcusrc/l3app/sensortemp.c \
/home/test/workspace/hcu/hcusrc/l3app/sensortoxicgas.c \
/home/test/workspace/hcu/hcusrc/l3app/sensorwinddir.c \
/home/test/workspace/hcu/hcusrc/l3app/sensorwindspd.c \
/home/test/workspace/hcu/hcusrc/l3app/svrcon.c \
/home/test/workspace/hcu/hcusrc/l3app/syspm.c \
/home/test/workspace/hcu/hcusrc/l3app/sysswm.c 

OBJS += \
./src/l3app/hsmmp.o \
./src/l3app/sensorairprs.o \
./src/l3app/sensoralcohol.o \
./src/l3app/sensorco1.o \
./src/l3app/sensoremc.o \
./src/l3app/sensorhcho.o \
./src/l3app/sensorhumid.o \
./src/l3app/sensorigm.o \
./src/l3app/sensorihm.o \
./src/l3app/sensoripm.o \
./src/l3app/sensoriwm.o \
./src/l3app/sensorlightstr.o \
./src/l3app/sensornoise.o \
./src/l3app/sensorpm25.o \
./src/l3app/sensorpm25sharp.o \
./src/l3app/sensortemp.o \
./src/l3app/sensortoxicgas.o \
./src/l3app/sensorwinddir.o \
./src/l3app/sensorwindspd.o \
./src/l3app/svrcon.o \
./src/l3app/syspm.o \
./src/l3app/sysswm.o 

C_DEPS += \
./src/l3app/hsmmp.d \
./src/l3app/sensorairprs.d \
./src/l3app/sensoralcohol.d \
./src/l3app/sensorco1.d \
./src/l3app/sensoremc.d \
./src/l3app/sensorhcho.d \
./src/l3app/sensorhumid.d \
./src/l3app/sensorigm.d \
./src/l3app/sensorihm.d \
./src/l3app/sensoripm.d \
./src/l3app/sensoriwm.d \
./src/l3app/sensorlightstr.d \
./src/l3app/sensornoise.d \
./src/l3app/sensorpm25.d \
./src/l3app/sensorpm25sharp.d \
./src/l3app/sensortemp.d \
./src/l3app/sensortoxicgas.d \
./src/l3app/sensorwinddir.d \
./src/l3app/sensorwindspd.d \
./src/l3app/svrcon.d \
./src/l3app/syspm.d \
./src/l3app/sysswm.d 


# Each subdirectory must supply rules for building sources it contributes
src/l3app/hsmmp.o: /home/test/workspace/hcu/hcusrc/l3app/hsmmp.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sensorairprs.o: /home/test/workspace/hcu/hcusrc/l3app/sensorairprs.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sensoralcohol.o: /home/test/workspace/hcu/hcusrc/l3app/sensoralcohol.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sensorco1.o: /home/test/workspace/hcu/hcusrc/l3app/sensorco1.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sensoremc.o: /home/test/workspace/hcu/hcusrc/l3app/sensoremc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sensorhcho.o: /home/test/workspace/hcu/hcusrc/l3app/sensorhcho.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sensorhumid.o: /home/test/workspace/hcu/hcusrc/l3app/sensorhumid.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sensorigm.o: /home/test/workspace/hcu/hcusrc/l3app/sensorigm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sensorihm.o: /home/test/workspace/hcu/hcusrc/l3app/sensorihm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sensoripm.o: /home/test/workspace/hcu/hcusrc/l3app/sensoripm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sensoriwm.o: /home/test/workspace/hcu/hcusrc/l3app/sensoriwm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sensorlightstr.o: /home/test/workspace/hcu/hcusrc/l3app/sensorlightstr.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sensornoise.o: /home/test/workspace/hcu/hcusrc/l3app/sensornoise.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sensorpm25.o: /home/test/workspace/hcu/hcusrc/l3app/sensorpm25.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sensorpm25sharp.o: /home/test/workspace/hcu/hcusrc/l3app/sensorpm25sharp.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sensortemp.o: /home/test/workspace/hcu/hcusrc/l3app/sensortemp.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sensortoxicgas.o: /home/test/workspace/hcu/hcusrc/l3app/sensortoxicgas.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sensorwinddir.o: /home/test/workspace/hcu/hcusrc/l3app/sensorwinddir.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sensorwindspd.o: /home/test/workspace/hcu/hcusrc/l3app/sensorwindspd.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/svrcon.o: /home/test/workspace/hcu/hcusrc/l3app/svrcon.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/syspm.o: /home/test/workspace/hcu/hcusrc/l3app/syspm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l3app/sysswm.o: /home/test/workspace/hcu/hcusrc/l3app/sysswm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


