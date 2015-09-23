################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/nRF24L01P/nRF24L01P.c \
../src/nRF24L01P/nRF24L01P_SPI.c \
../src/nRF24L01P/stm32f4_discovery.c 

OBJS += \
./src/nRF24L01P/nRF24L01P.o \
./src/nRF24L01P/nRF24L01P_SPI.o \
./src/nRF24L01P/stm32f4_discovery.o 

C_DEPS += \
./src/nRF24L01P/nRF24L01P.d \
./src/nRF24L01P/nRF24L01P_SPI.d \
./src/nRF24L01P/stm32f4_discovery.d 


# Each subdirectory must supply rules for building sources it contributes
src/nRF24L01P/%.o: ../src/nRF24L01P/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -DDEBUG -I"../libs/misc/include" -I"../include" -I"../libs/CMSIS/include" -I"../libs/StdPeriph/include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


