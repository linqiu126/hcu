################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/test/workspace/hcu/hcusrc/l0dbi/dbi_sysswm.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbiairprs.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbialcohol.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbico1.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbicom.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbiemc.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbihcho.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbihsmmp.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbihumid.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbiigm.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbiihm.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbiipm.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbiiwm.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbilightstr.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbinbiot.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbinoise.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbipm25.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbisysperfm.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbitemp.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbitoxicgas.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbiwinddir.c \
/home/test/workspace/hcu/hcusrc/l0dbi/dbiwindspd.c 

OBJS += \
./src/l0dbi/dbi_sysswm.o \
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
./src/l0dbi/dbi_sysswm.d \
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
src/l0dbi/dbi_sysswm.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbi_sysswm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbiairprs.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbiairprs.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbialcohol.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbialcohol.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbico1.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbico1.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbicom.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbicom.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbiemc.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbiemc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbihcho.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbihcho.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbihsmmp.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbihsmmp.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbihumid.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbihumid.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbiigm.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbiigm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbiihm.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbiihm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbiipm.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbiipm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbiiwm.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbiiwm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbilightstr.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbilightstr.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbinbiot.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbinbiot.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbinoise.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbinoise.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbipm25.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbipm25.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbisysperfm.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbisysperfm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbitemp.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbitemp.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbitoxicgas.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbitoxicgas.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbiwinddir.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbiwinddir.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l0dbi/dbiwindspd.o: /home/test/workspace/hcu/hcusrc/l0dbi/dbiwindspd.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

