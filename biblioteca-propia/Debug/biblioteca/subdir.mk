################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../biblioteca/paquetes.c \
../biblioteca/serializacion.c \
../biblioteca/sockets.c 

OBJS += \
./biblioteca/paquetes.o \
./biblioteca/serializacion.o \
./biblioteca/sockets.o 

C_DEPS += \
./biblioteca/paquetes.d \
./biblioteca/serializacion.d \
./biblioteca/sockets.d 


# Each subdirectory must supply rules for building sources it contributes
biblioteca/%.o: ../biblioteca/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


