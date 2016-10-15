################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../L2_Drivers/base/char_dev.cpp \
../L2_Drivers/base/i2c_base.cpp \
../L2_Drivers/base/uart_dev.cpp 

OBJS += \
./L2_Drivers/base/char_dev.o \
./L2_Drivers/base/i2c_base.o \
./L2_Drivers/base/uart_dev.o 

CPP_DEPS += \
./L2_Drivers/base/char_dev.d \
./L2_Drivers/base/i2c_base.d \
./L2_Drivers/base/uart_dev.d 


# Each subdirectory must supply rules for building sources it contributes
L2_Drivers/base/%.o: ../L2_Drivers/base/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wshadow -Wlogical-op -Wfloat-equal -DBUILD_CFG_MPU=0 -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\newlib" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L0_LowLevel" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L1_FreeRTOS" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L1_FreeRTOS\include" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L1_FreeRTOS\portable" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L1_FreeRTOS\portable\no_mpu" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L2_Drivers" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L2_Drivers\base" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L3_Utils" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L3_Utils\tlm" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L4_IO" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L4_IO\fat" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L4_IO\wireless" -I"C:\Users\ythao\Documents\cmpe243_project\TITANS_PROJECT_RC_CAR\L5_Application" -std=gnu++11 -fabi-version=0 -fno-exceptions -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


