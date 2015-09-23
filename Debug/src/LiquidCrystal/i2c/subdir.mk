################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/LiquidCrystal/i2c/stm32f4_i2c_Driver.c 

OBJS += \
./src/LiquidCrystal/i2c/stm32f4_i2c_Driver.o 

C_DEPS += \
./src/LiquidCrystal/i2c/stm32f4_i2c_Driver.d 


# Each subdirectory must supply rules for building sources it contributes
src/LiquidCrystal/i2c/%.o: ../src/LiquidCrystal/i2c/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -DDEBUG -I"../libs/misc/include" -I"../include" -I"../libs/CMSIS/include" -I"../libs/StdPeriph/include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


