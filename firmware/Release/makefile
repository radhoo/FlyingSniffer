################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include misc/subdir.mk
-include devices/system/uart/subdir.mk
-include devices/system/time/subdir.mk
-include devices/system/spi/subdir.mk
-include devices/system/i2c/subdir.mk
-include devices/system/gpio/subdir.mk
-include devices/system/adc/subdir.mk
-include devices/sensors/zh03a/subdir.mk
-include devices/sensors/geiger/subdir.mk
-include devices/sensors/bme680/subdir.mk
-include devices/external/speaker/subdir.mk
-include devices/external/sdcard/utility/subdir.mk
-include devices/external/sdcard/subdir.mk
-include devices/external/rtc/subdir.mk
-include devices/external/nmea/subdir.mk
-include devices/external/morse/subdir.mk
-include devices/external/inverter/subdir.mk
-include devices/external/battery/subdir.mk
-include devices/data/subdir.mk
-include devices/subdir.mk
-include app/subdir.mk
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
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
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
LSS += \
uradmonitor-Drone.lss \

FLASH_IMAGE += \
uradmonitor-Drone.hex \

EEPROM_IMAGE += \
uradmonitor-Drone.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: uradmonitor-Drone.elf secondary-outputs

# Tool invocations
uradmonitor-Drone.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C++ Linker'
	avr-g++ -Wl,-Map,uradmonitor-Drone.map,--cref -mrelax -Wl,--gc-sections -Wl,-u,vfprintf -lprintf_flt -lm -Llibprintf_flt.a -Llibm.a -mmcu=atmega2561 -o "uradmonitor-Drone.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

uradmonitor-Drone.lss: uradmonitor-Drone.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S uradmonitor-Drone.elf  >"uradmonitor-Drone.lss"
	@echo 'Finished building: $@'
	@echo ' '

uradmonitor-Drone.hex: uradmonitor-Drone.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex uradmonitor-Drone.elf  "uradmonitor-Drone.hex"
	@echo 'Finished building: $@'
	@echo ' '

uradmonitor-Drone.eep: uradmonitor-Drone.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex uradmonitor-Drone.elf  "uradmonitor-Drone.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: uradmonitor-Drone.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega2561 uradmonitor-Drone.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(S_DEPS)$(LSS)$(C_DEPS)$(FLASH_IMAGE)$(CC_DEPS)$(ELFS)$(C++_DEPS)$(OBJS)$(CXX_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(SIZEDUMMY)$(S_UPPER_DEPS)$(CPP_DEPS) uradmonitor-Drone.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
