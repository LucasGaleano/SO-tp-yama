################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/consola.c \
../src/fileSystem.c \
../src/tablas.c 

OBJS += \
./src/consola.o \
./src/fileSystem.o \
./src/tablas.o 

C_DEPS += \
./src/consola.d \
./src/fileSystem.d \
./src/tablas.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2017-2c-NULL/biblioteca-propia" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


