################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/comunicacion_CPU.c \
../src/comunicacion_KERNEL.c \
../src/init_memswap.c \


OBJS += \
../src/comunicacion_CPU.o \
../src/comunicacion_KERNEL.o \
../src/init_memswap.o \

C_DEPS += \
../src/comunicacion_CPU.d \
../src/comunicacion_KERNEL.d \
../src/init_memswap.d \

# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


