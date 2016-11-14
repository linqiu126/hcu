################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/l4bi/subdir.mk
-include src/l3appaqycg10/subdir.mk
-include src/l3app/subdir.mk
-include src/l2frame/subdir.mk
-include src/l1hwopt/usb_adc/subdir.mk
-include src/l1hwopt/subdir.mk
-include src/l1com/subdir.mk
-include src/l0webser/subdir.mk
-include src/l0service/subdir.mk
-include src/l0main/subdir.mk
-include src/l0dbi/subdir.mk
-include src/l0comvm/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: PrjAqycG10

# Tool invocations
PrjAqycG10: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	g++ -L/usr/local/lib -L/root/ffmpeg_build/lib -L/usr/local/sqlite3/lib -o "PrjAqycG10" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(C_DEPS)$(CPP_DEPS) PrjAqycG10
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
