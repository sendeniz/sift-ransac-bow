################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../cvlib/flann/flann.cpp \
../cvlib/flann/miniflann.cpp 

OBJS += \
./cvlib/flann/flann.o \
./cvlib/flann/miniflann.o 

CPP_DEPS += \
./cvlib/flann/flann.d \
./cvlib/flann/miniflann.d 


# Each subdirectory must supply rules for building sources it contributes
cvlib/flann/%.o: ../cvlib/flann/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -w -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


