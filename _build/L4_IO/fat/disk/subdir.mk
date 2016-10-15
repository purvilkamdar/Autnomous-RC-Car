################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../L4_IO/fat/disk/spi_flash.cpp 

C_SRCS += \
../L4_IO/fat/disk/diskio.c \
../L4_IO/fat/disk/sd.c 

OBJS += \
./L4_IO/fat/disk/diskio.o \
./L4_IO/fat/disk/sd.o \
./L4_IO/fat/disk/spi_flash.o 

C_DEPS += \
./L4_IO/fat/disk/diskio.d \
./L4_IO/fat/disk/sd.d 

CPP_DEPS += \
./L4_IO/fat/disk/spi_flash.d 


# Each subdirectory must supply rules for building sources it contributes
L4_IO/fat/disk/%.o: ../L4_IO/fat/disk/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wshadow -Wlogical-op -Wfloat-equal -DBUILD_CFG_MPU=0 -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\newlib" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L0_LowLevel" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L1_FreeRTOS" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L1_FreeRTOS\include" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L1_FreeRTOS\portable\no_mpu" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L2_Drivers" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L2_Drivers\base" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L3_Utils" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L3_Utils\tlm" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L4_IO" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L4_IO\fat" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L4_IO\wireless" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L5_Application" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

L4_IO/fat/disk/%.o: ../L4_IO/fat/disk/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wshadow -Wlogical-op -Wfloat-equal -DBUILD_CFG_MPU=0 -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\newlib" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L0_LowLevel" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L1_FreeRTOS" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L1_FreeRTOS\include" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L1_FreeRTOS\portable" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L1_FreeRTOS\portable\no_mpu" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L2_Drivers" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L2_Drivers\base" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L3_Utils" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L3_Utils\tlm" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L4_IO" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L4_IO\fat" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L4_IO\wireless" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L5_Application" -std=gnu++11 -fabi-version=0 -fno-exceptions -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


