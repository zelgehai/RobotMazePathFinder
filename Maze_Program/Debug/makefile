################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /Applications/ti/ccs1260/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./main.obj" \
"./startup_msp432p401r_ccs.obj" \
"./system_msp432p401r.obj" \
"./src/Analog_Distance_Sensors.obj" \
"./src/Barcode_Scanner.obj" \
"./src/Bumper_Switches.obj" \
"./src/Buzzer.obj" \
"./src/Clock.obj" \
"./src/CortexM.obj" \
"./src/EUSCI_A0_UART.obj" \
"./src/EUSCI_A3_UART.obj" \
"./src/EUSCI_B1_I2C.obj" \
"./src/GPIO.obj" \
"./src/LPF.obj" \
"./src/Motor.obj" \
"./src/OPT3001.obj" \
"./src/OPT3101.obj" \
"./src/Print_Binary.obj" \
"./src/Reflectance_Sensor.obj" \
"./src/SysTick_Interrupt.obj" \
"./src/Tachometer.obj" \
"./src/Timer_A0_Interrupt.obj" \
"./src/Timer_A0_PWM.obj" \
"./src/Timer_A1_Interrupt.obj" \
"./src/Timer_A2_Capture.obj" \
"./src/Timer_A2_Interrupt.obj" \
"./src/Timer_A2_PWM.obj" \
"./src/Timer_A3_Capture.obj" \
"../msp432p401r.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include src/subdir_vars.mk
-include subdir_rules.mk
-include src/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
Object_Detection.out 

EXE_OUTPUTS__QUOTED += \
"Object_Detection.out" 


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "Object_Detection.out"

# Tool invocations
Object_Detection.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Linker'
	"/Applications/ti/ccs1260/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --advice:power=all --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi -z -m"Object_Detection.map" --heap_size=1024 --stack_size=512 -i"/Applications/ti/ccs1260/ccs/ccs_base/arm/include" -i"/Applications/ti/ccs1260/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/lib" -i"/Applications/ti/ccs1260/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="Object_Detection_linkInfo.xml" --rom_model -o "Object_Detection.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)
	-$(RM) "main.obj" "startup_msp432p401r_ccs.obj" "system_msp432p401r.obj" "src/Analog_Distance_Sensors.obj" "src/Barcode_Scanner.obj" "src/Bumper_Switches.obj" "src/Buzzer.obj" "src/Clock.obj" "src/CortexM.obj" "src/EUSCI_A0_UART.obj" "src/EUSCI_A3_UART.obj" "src/EUSCI_B1_I2C.obj" "src/GPIO.obj" "src/LPF.obj" "src/Motor.obj" "src/OPT3001.obj" "src/OPT3101.obj" "src/Print_Binary.obj" "src/Reflectance_Sensor.obj" "src/SysTick_Interrupt.obj" "src/Tachometer.obj" "src/Timer_A0_Interrupt.obj" "src/Timer_A0_PWM.obj" "src/Timer_A1_Interrupt.obj" "src/Timer_A2_Capture.obj" "src/Timer_A2_Interrupt.obj" "src/Timer_A2_PWM.obj" "src/Timer_A3_Capture.obj" 
	-$(RM) "main.d" "startup_msp432p401r_ccs.d" "system_msp432p401r.d" "src/Analog_Distance_Sensors.d" "src/Barcode_Scanner.d" "src/Bumper_Switches.d" "src/Buzzer.d" "src/Clock.d" "src/CortexM.d" "src/EUSCI_A0_UART.d" "src/EUSCI_A3_UART.d" "src/EUSCI_B1_I2C.d" "src/GPIO.d" "src/LPF.d" "src/Motor.d" "src/OPT3001.d" "src/OPT3101.d" "src/Print_Binary.d" "src/Reflectance_Sensor.d" "src/SysTick_Interrupt.d" "src/Tachometer.d" "src/Timer_A0_Interrupt.d" "src/Timer_A0_PWM.d" "src/Timer_A1_Interrupt.d" "src/Timer_A2_Capture.d" "src/Timer_A2_Interrupt.d" "src/Timer_A2_PWM.d" "src/Timer_A3_Capture.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

