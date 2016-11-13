################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/l4bi/subdir.mk
-include src/l3app/subdir.mk
-include src/l2frame/subdir.mk
-include src/l1hwopt/usb_adc/subdir.mk
-include src/l1hwopt/subdir.mk
-include src/l1com/subdir.mk
-include src/l0webser/subdir.mk
-include src/l0service/subdir.mk
-include src/l0dbi/subdir.mk
-include src/l0comvm/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: hcu

# Tool invocations
hcu: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	gcc -L/usr/local/lib -L/root/ffmpeg_build/lib -L/usr/local/sqlite3/lib -o "hcu" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) hcu
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
