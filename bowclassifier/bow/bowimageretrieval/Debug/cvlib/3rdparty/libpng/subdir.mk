################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../cvlib/3rdparty/libpng/png.c \
../cvlib/3rdparty/libpng/pngerror.c \
../cvlib/3rdparty/libpng/pngget.c \
../cvlib/3rdparty/libpng/pngmem.c \
../cvlib/3rdparty/libpng/pngpread.c \
../cvlib/3rdparty/libpng/pngread.c \
../cvlib/3rdparty/libpng/pngrio.c \
../cvlib/3rdparty/libpng/pngrtran.c \
../cvlib/3rdparty/libpng/pngrutil.c \
../cvlib/3rdparty/libpng/pngset.c \
../cvlib/3rdparty/libpng/pngtrans.c \
../cvlib/3rdparty/libpng/pngwio.c \
../cvlib/3rdparty/libpng/pngwrite.c \
../cvlib/3rdparty/libpng/pngwtran.c \
../cvlib/3rdparty/libpng/pngwutil.c 

OBJS += \
./cvlib/3rdparty/libpng/png.o \
./cvlib/3rdparty/libpng/pngerror.o \
./cvlib/3rdparty/libpng/pngget.o \
./cvlib/3rdparty/libpng/pngmem.o \
./cvlib/3rdparty/libpng/pngpread.o \
./cvlib/3rdparty/libpng/pngread.o \
./cvlib/3rdparty/libpng/pngrio.o \
./cvlib/3rdparty/libpng/pngrtran.o \
./cvlib/3rdparty/libpng/pngrutil.o \
./cvlib/3rdparty/libpng/pngset.o \
./cvlib/3rdparty/libpng/pngtrans.o \
./cvlib/3rdparty/libpng/pngwio.o \
./cvlib/3rdparty/libpng/pngwrite.o \
./cvlib/3rdparty/libpng/pngwtran.o \
./cvlib/3rdparty/libpng/pngwutil.o 

C_DEPS += \
./cvlib/3rdparty/libpng/png.d \
./cvlib/3rdparty/libpng/pngerror.d \
./cvlib/3rdparty/libpng/pngget.d \
./cvlib/3rdparty/libpng/pngmem.d \
./cvlib/3rdparty/libpng/pngpread.d \
./cvlib/3rdparty/libpng/pngread.d \
./cvlib/3rdparty/libpng/pngrio.d \
./cvlib/3rdparty/libpng/pngrtran.d \
./cvlib/3rdparty/libpng/pngrutil.d \
./cvlib/3rdparty/libpng/pngset.d \
./cvlib/3rdparty/libpng/pngtrans.d \
./cvlib/3rdparty/libpng/pngwio.d \
./cvlib/3rdparty/libpng/pngwrite.d \
./cvlib/3rdparty/libpng/pngwtran.d \
./cvlib/3rdparty/libpng/pngwutil.d 


# Each subdirectory must supply rules for building sources it contributes
cvlib/3rdparty/libpng/%.o: ../cvlib/3rdparty/libpng/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -w -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


