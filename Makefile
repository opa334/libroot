CC = clang

SRC_FILES := $(wildcard src/*.c)

IPHONEOS_ARM_BUILD_DIR := build/iphoneos_arm
IPHONEOS_ARM_OBJ_FILES := $(patsubst src/%.c,$(IPHONEOS_ARM_BUILD_DIR)/%.o,$(SRC_FILES))

IPHONEOS_ARM64_BUILD_DIR := build/iphoneos_arm64
IPHONEOS_ARM64_OBJ_FILES := $(patsubst src/%.c,$(IPHONEOS_ARM64_BUILD_DIR)/%.o,$(SRC_FILES))

CFLAGS = -I../../BaseBin/.include -Isrc -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -arch armv7 -arch arm64 -arch arm64e -miphoneos-version-min=7.0 -fobjc-arc -O2
LDFLAGS =

all: libroot_dyn_iphoneos-arm.a libroot_dyn_iphoneos-arm64.a

libroot_dyn_iphoneos-arm.a: $(IPHONEOS_ARM_OBJ_FILES)
	@libtool $^ -o $@

libroot_dyn_iphoneos-arm64.a: $(IPHONEOS_ARM64_OBJ_FILES)
	@libtool $^ -o $@

$(IPHONEOS_ARM_BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

$(IPHONEOS_ARM64_BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DIPHONEOS_ARM64 $(LDFLAGS) -c $< -o $@

clean:
	@rm -rf build
	@rm -f libroot_dyn_iphoneos-arm.a
	@rm -f libroot_dyn_iphoneos-arm64.a