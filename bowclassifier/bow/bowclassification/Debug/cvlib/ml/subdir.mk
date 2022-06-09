################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../cvlib/ml/ann_mlp.cpp \
../cvlib/ml/boost.cpp \
../cvlib/ml/cnn.cpp \
../cvlib/ml/data.cpp \
../cvlib/ml/em.cpp \
../cvlib/ml/ertrees.cpp \
../cvlib/ml/estimate.cpp \
../cvlib/ml/gbt.cpp \
../cvlib/ml/inner_functions.cpp \
../cvlib/ml/knearest.cpp \
../cvlib/ml/nbayes.cpp \
../cvlib/ml/rtrees.cpp \
../cvlib/ml/svm.cpp \
../cvlib/ml/testset.cpp \
../cvlib/ml/tree.cpp 

OBJS += \
./cvlib/ml/ann_mlp.o \
./cvlib/ml/boost.o \
./cvlib/ml/cnn.o \
./cvlib/ml/data.o \
./cvlib/ml/em.o \
./cvlib/ml/ertrees.o \
./cvlib/ml/estimate.o \
./cvlib/ml/gbt.o \
./cvlib/ml/inner_functions.o \
./cvlib/ml/knearest.o \
./cvlib/ml/nbayes.o \
./cvlib/ml/rtrees.o \
./cvlib/ml/svm.o \
./cvlib/ml/testset.o \
./cvlib/ml/tree.o 

CPP_DEPS += \
./cvlib/ml/ann_mlp.d \
./cvlib/ml/boost.d \
./cvlib/ml/cnn.d \
./cvlib/ml/data.d \
./cvlib/ml/em.d \
./cvlib/ml/ertrees.d \
./cvlib/ml/estimate.d \
./cvlib/ml/gbt.d \
./cvlib/ml/inner_functions.d \
./cvlib/ml/knearest.d \
./cvlib/ml/nbayes.d \
./cvlib/ml/rtrees.d \
./cvlib/ml/svm.d \
./cvlib/ml/testset.d \
./cvlib/ml/tree.d 


# Each subdirectory must supply rules for building sources it contributes
cvlib/ml/%.o: ../cvlib/ml/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -w -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


