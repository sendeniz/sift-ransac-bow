################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../cvlib/highgui/bitstrm.cpp \
../cvlib/highgui/grfmt_base.cpp \
../cvlib/highgui/grfmt_bmp.cpp \
../cvlib/highgui/grfmt_jpeg.cpp \
../cvlib/highgui/grfmt_png.cpp \
../cvlib/highgui/loadsave.cpp \
../cvlib/highgui/utils.cpp 

OBJS += \
./cvlib/highgui/bitstrm.o \
./cvlib/highgui/grfmt_base.o \
./cvlib/highgui/grfmt_bmp.o \
./cvlib/highgui/grfmt_jpeg.o \
./cvlib/highgui/grfmt_png.o \
./cvlib/highgui/loadsave.o \
./cvlib/highgui/utils.o 

CPP_DEPS += \
./cvlib/highgui/bitstrm.d \
./cvlib/highgui/grfmt_base.d \
./cvlib/highgui/grfmt_bmp.d \
./cvlib/highgui/grfmt_jpeg.d \
./cvlib/highgui/grfmt_png.d \
./cvlib/highgui/loadsave.d \
./cvlib/highgui/utils.d 


# Each subdirectory must supply rules for building sources it contributes
cvlib/highgui/%.o: ../cvlib/highgui/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -w -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


