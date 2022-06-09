################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../cvlib/3rdparty/zlib/adler32.c \
../cvlib/3rdparty/zlib/compress.c \
../cvlib/3rdparty/zlib/crc32.c \
../cvlib/3rdparty/zlib/deflate.c \
../cvlib/3rdparty/zlib/gzclose.c \
../cvlib/3rdparty/zlib/gzlib.c \
../cvlib/3rdparty/zlib/gzread.c \
../cvlib/3rdparty/zlib/gzwrite.c \
../cvlib/3rdparty/zlib/infback.c \
../cvlib/3rdparty/zlib/inffast.c \
../cvlib/3rdparty/zlib/inflate.c \
../cvlib/3rdparty/zlib/inftrees.c \
../cvlib/3rdparty/zlib/trees.c \
../cvlib/3rdparty/zlib/uncompr.c \
../cvlib/3rdparty/zlib/zutil.c 

OBJS += \
./cvlib/3rdparty/zlib/adler32.o \
./cvlib/3rdparty/zlib/compress.o \
./cvlib/3rdparty/zlib/crc32.o \
./cvlib/3rdparty/zlib/deflate.o \
./cvlib/3rdparty/zlib/gzclose.o \
./cvlib/3rdparty/zlib/gzlib.o \
./cvlib/3rdparty/zlib/gzread.o \
./cvlib/3rdparty/zlib/gzwrite.o \
./cvlib/3rdparty/zlib/infback.o \
./cvlib/3rdparty/zlib/inffast.o \
./cvlib/3rdparty/zlib/inflate.o \
./cvlib/3rdparty/zlib/inftrees.o \
./cvlib/3rdparty/zlib/trees.o \
./cvlib/3rdparty/zlib/uncompr.o \
./cvlib/3rdparty/zlib/zutil.o 

C_DEPS += \
./cvlib/3rdparty/zlib/adler32.d \
./cvlib/3rdparty/zlib/compress.d \
./cvlib/3rdparty/zlib/crc32.d \
./cvlib/3rdparty/zlib/deflate.d \
./cvlib/3rdparty/zlib/gzclose.d \
./cvlib/3rdparty/zlib/gzlib.d \
./cvlib/3rdparty/zlib/gzread.d \
./cvlib/3rdparty/zlib/gzwrite.d \
./cvlib/3rdparty/zlib/infback.d \
./cvlib/3rdparty/zlib/inffast.d \
./cvlib/3rdparty/zlib/inflate.d \
./cvlib/3rdparty/zlib/inftrees.d \
./cvlib/3rdparty/zlib/trees.d \
./cvlib/3rdparty/zlib/uncompr.d \
./cvlib/3rdparty/zlib/zutil.d 


# Each subdirectory must supply rules for building sources it contributes
cvlib/3rdparty/zlib/%.o: ../cvlib/3rdparty/zlib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -w -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


