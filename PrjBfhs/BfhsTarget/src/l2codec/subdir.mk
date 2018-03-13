################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/hitpony/workspace/hcu/hcusrc/l2codec/huicobuscodec.c \
/home/hitpony/workspace/hcu/hcusrc/l2codec/huijsoncodec.c \
/home/hitpony/workspace/hcu/hcusrc/l2codec/huirestcodec.c \
/home/hitpony/workspace/hcu/hcusrc/l2codec/huixmlcodec.c \
/home/hitpony/workspace/hcu/hcusrc/l2codec/zhbhjt212codec.c 

OBJS += \
./src/l2codec/huicobuscodec.o \
./src/l2codec/huijsoncodec.o \
./src/l2codec/huirestcodec.o \
./src/l2codec/huixmlcodec.o \
./src/l2codec/zhbhjt212codec.o 

C_DEPS += \
./src/l2codec/huicobuscodec.d \
./src/l2codec/huijsoncodec.d \
./src/l2codec/huirestcodec.d \
./src/l2codec/huixmlcodec.d \
./src/l2codec/zhbhjt212codec.d 


# Each subdirectory must supply rules for building sources it contributes
src/l2codec/huicobuscodec.o: /home/hitpony/workspace/hcu/hcusrc/l2codec/huicobuscodec.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2codec/huijsoncodec.o: /home/hitpony/workspace/hcu/hcusrc/l2codec/huijsoncodec.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2codec/huirestcodec.o: /home/hitpony/workspace/hcu/hcusrc/l2codec/huirestcodec.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2codec/huixmlcodec.o: /home/hitpony/workspace/hcu/hcusrc/l2codec/huixmlcodec.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/l2codec/zhbhjt212codec.o: /home/hitpony/workspace/hcu/hcusrc/l2codec/zhbhjt212codec.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DTARGET_LINUX_X86 -DTARGET_LINUX_X86_ADVANTECH -I/usr/include/libxml2 -I/usr/include/x86_64-linux-gnu/ -I/usr/include -I/usr/local/sqlite3/include -O0 -lpthread -g3 -Wall -c -fmessage-length=0 -Wl,--hash-style=sysv -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


