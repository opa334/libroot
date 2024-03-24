make clean-artifacts
TOOLCHAIN_PREFIX=$THEOS/toolchain/Xcode11.xctoolchain/usr/bin/ make

mv libroot_dyn_iphoneos-arm.a libroot_dyn_iphoneos-arm.oldabi.a
mv libroot_dyn_iphoneos-arm64.a libroot_dyn_iphoneos-arm64.oldabi.a

make clean
make