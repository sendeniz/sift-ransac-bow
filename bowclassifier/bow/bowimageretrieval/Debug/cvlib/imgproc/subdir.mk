################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../cvlib/imgproc/accum.cpp \
../cvlib/imgproc/approx.cpp \
../cvlib/imgproc/canny.cpp \
../cvlib/imgproc/color.cpp \
../cvlib/imgproc/contours.cpp \
../cvlib/imgproc/convhull.cpp \
../cvlib/imgproc/corner.cpp \
../cvlib/imgproc/cornersubpix.cpp \
../cvlib/imgproc/deriv.cpp \
../cvlib/imgproc/distransform.cpp \
../cvlib/imgproc/filter.cpp \
../cvlib/imgproc/floodfill.cpp \
../cvlib/imgproc/geometry.cpp \
../cvlib/imgproc/grabcut.cpp \
../cvlib/imgproc/histogram.cpp \
../cvlib/imgproc/hough.cpp \
../cvlib/imgproc/imgwarp.cpp \
../cvlib/imgproc/inpaint.cpp \
../cvlib/imgproc/linefit.cpp \
../cvlib/imgproc/matchcontours.cpp \
../cvlib/imgproc/moments.cpp \
../cvlib/imgproc/morph.cpp \
../cvlib/imgproc/pyramids.cpp \
../cvlib/imgproc/rotcalipers.cpp \
../cvlib/imgproc/samplers.cpp \
../cvlib/imgproc/shapedescr.cpp \
../cvlib/imgproc/smooth.cpp \
../cvlib/imgproc/subdivision2d.cpp \
../cvlib/imgproc/sumpixels.cpp \
../cvlib/imgproc/templmatch.cpp \
../cvlib/imgproc/thresh.cpp \
../cvlib/imgproc/undistort.cpp \
../cvlib/imgproc/utils2.cpp 

OBJS += \
./cvlib/imgproc/accum.o \
./cvlib/imgproc/approx.o \
./cvlib/imgproc/canny.o \
./cvlib/imgproc/color.o \
./cvlib/imgproc/contours.o \
./cvlib/imgproc/convhull.o \
./cvlib/imgproc/corner.o \
./cvlib/imgproc/cornersubpix.o \
./cvlib/imgproc/deriv.o \
./cvlib/imgproc/distransform.o \
./cvlib/imgproc/filter.o \
./cvlib/imgproc/floodfill.o \
./cvlib/imgproc/geometry.o \
./cvlib/imgproc/grabcut.o \
./cvlib/imgproc/histogram.o \
./cvlib/imgproc/hough.o \
./cvlib/imgproc/imgwarp.o \
./cvlib/imgproc/inpaint.o \
./cvlib/imgproc/linefit.o \
./cvlib/imgproc/matchcontours.o \
./cvlib/imgproc/moments.o \
./cvlib/imgproc/morph.o \
./cvlib/imgproc/pyramids.o \
./cvlib/imgproc/rotcalipers.o \
./cvlib/imgproc/samplers.o \
./cvlib/imgproc/shapedescr.o \
./cvlib/imgproc/smooth.o \
./cvlib/imgproc/subdivision2d.o \
./cvlib/imgproc/sumpixels.o \
./cvlib/imgproc/templmatch.o \
./cvlib/imgproc/thresh.o \
./cvlib/imgproc/undistort.o \
./cvlib/imgproc/utils2.o 

CPP_DEPS += \
./cvlib/imgproc/accum.d \
./cvlib/imgproc/approx.d \
./cvlib/imgproc/canny.d \
./cvlib/imgproc/color.d \
./cvlib/imgproc/contours.d \
./cvlib/imgproc/convhull.d \
./cvlib/imgproc/corner.d \
./cvlib/imgproc/cornersubpix.d \
./cvlib/imgproc/deriv.d \
./cvlib/imgproc/distransform.d \
./cvlib/imgproc/filter.d \
./cvlib/imgproc/floodfill.d \
./cvlib/imgproc/geometry.d \
./cvlib/imgproc/grabcut.d \
./cvlib/imgproc/histogram.d \
./cvlib/imgproc/hough.d \
./cvlib/imgproc/imgwarp.d \
./cvlib/imgproc/inpaint.d \
./cvlib/imgproc/linefit.d \
./cvlib/imgproc/matchcontours.d \
./cvlib/imgproc/moments.d \
./cvlib/imgproc/morph.d \
./cvlib/imgproc/pyramids.d \
./cvlib/imgproc/rotcalipers.d \
./cvlib/imgproc/samplers.d \
./cvlib/imgproc/shapedescr.d \
./cvlib/imgproc/smooth.d \
./cvlib/imgproc/subdivision2d.d \
./cvlib/imgproc/sumpixels.d \
./cvlib/imgproc/templmatch.d \
./cvlib/imgproc/thresh.d \
./cvlib/imgproc/undistort.d \
./cvlib/imgproc/utils2.d 


# Each subdirectory must supply rules for building sources it contributes
cvlib/imgproc/%.o: ../cvlib/imgproc/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -w -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


