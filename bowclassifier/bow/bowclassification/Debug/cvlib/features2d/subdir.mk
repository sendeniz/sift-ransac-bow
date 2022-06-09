################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../cvlib/features2d/descriptors.cpp \
../cvlib/features2d/detectors.cpp \
../cvlib/features2d/draw.cpp \
../cvlib/features2d/keypoint.cpp \
../cvlib/features2d/matchers.cpp 

OBJS += \
./cvlib/features2d/descriptors.o \
./cvlib/features2d/detectors.o \
./cvlib/features2d/draw.o \
./cvlib/features2d/keypoint.o \
./cvlib/features2d/matchers.o 

CPP_DEPS += \
./cvlib/features2d/descriptors.d \
./cvlib/features2d/detectors.d \
./cvlib/features2d/draw.d \
./cvlib/features2d/keypoint.d \
./cvlib/features2d/matchers.d 


# Each subdirectory must supply rules for building sources it contributes
cvlib/features2d/%.o: ../cvlib/features2d/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -w -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


