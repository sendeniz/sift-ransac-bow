################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../cvlib/core/alloc.cpp \
../cvlib/core/arithm.cpp \
../cvlib/core/array.cpp \
../cvlib/core/cmdparser.cpp \
../cvlib/core/convert.cpp \
../cvlib/core/copy.cpp \
../cvlib/core/datastructs.cpp \
../cvlib/core/drawing.cpp \
../cvlib/core/dxt.cpp \
../cvlib/core/lapack.cpp \
../cvlib/core/mathfuncs.cpp \
../cvlib/core/matmul.cpp \
../cvlib/core/matop.cpp \
../cvlib/core/matrix.cpp \
../cvlib/core/out.cpp \
../cvlib/core/persistence.cpp \
../cvlib/core/rand.cpp \
../cvlib/core/stat.cpp \
../cvlib/core/system.cpp 

OBJS += \
./cvlib/core/alloc.o \
./cvlib/core/arithm.o \
./cvlib/core/array.o \
./cvlib/core/cmdparser.o \
./cvlib/core/convert.o \
./cvlib/core/copy.o \
./cvlib/core/datastructs.o \
./cvlib/core/drawing.o \
./cvlib/core/dxt.o \
./cvlib/core/lapack.o \
./cvlib/core/mathfuncs.o \
./cvlib/core/matmul.o \
./cvlib/core/matop.o \
./cvlib/core/matrix.o \
./cvlib/core/out.o \
./cvlib/core/persistence.o \
./cvlib/core/rand.o \
./cvlib/core/stat.o \
./cvlib/core/system.o 

CPP_DEPS += \
./cvlib/core/alloc.d \
./cvlib/core/arithm.d \
./cvlib/core/array.d \
./cvlib/core/cmdparser.d \
./cvlib/core/convert.d \
./cvlib/core/copy.d \
./cvlib/core/datastructs.d \
./cvlib/core/drawing.d \
./cvlib/core/dxt.d \
./cvlib/core/lapack.d \
./cvlib/core/mathfuncs.d \
./cvlib/core/matmul.d \
./cvlib/core/matop.d \
./cvlib/core/matrix.d \
./cvlib/core/out.d \
./cvlib/core/persistence.d \
./cvlib/core/rand.d \
./cvlib/core/stat.d \
./cvlib/core/system.d 


# Each subdirectory must supply rules for building sources it contributes
cvlib/core/%.o: ../cvlib/core/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -w -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


