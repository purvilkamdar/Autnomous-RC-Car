################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../newlib/memory.cpp 

C_SRCS += \
../newlib/malloc_lock.c \
../newlib/newlib_syscalls.c \
../newlib/newlib_time.c 

OBJS += \
./newlib/malloc_lock.o \
./newlib/memory.o \
./newlib/newlib_syscalls.o \
./newlib/newlib_time.o 

C_DEPS += \
./newlib/malloc_lock.d \
./newlib/newlib_syscalls.d \
./newlib/newlib_time.d 

CPP_DEPS += \
./newlib/memory.d 


# Each subdirectory must supply rules for building sources it contributes
newlib/%.o: ../newlib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wshadow -Wlogical-op -Wfloat-equal -DBUILD_CFG_MPU=0 -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\newlib" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L0_LowLevel" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L1_FreeRTOS" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L1_FreeRTOS\include" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L1_FreeRTOS\portable\no_mpu" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L2_Drivers" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L2_Drivers\base" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L3_Utils" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L3_Utils\tlm" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L4_IO" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L4_IO\fat" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L4_IO\wireless" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L5_Application" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

newlib/%.o: ../newlib/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wshadow -Wlogical-op -Wfloat-equal -DBUILD_CFG_MPU=0 -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\newlib" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L0_LowLevel" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L1_FreeRTOS" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L1_FreeRTOS\include" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L1_FreeRTOS\portable" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L1_FreeRTOS\portable\no_mpu" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L2_Drivers" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L2_Drivers\base" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L3_Utils" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L3_Utils\tlm" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L4_IO" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L4_IO\fat" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L4_IO\wireless" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L5_Application" -std=gnu++11 -fabi-version=0 -fno-exceptions -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


