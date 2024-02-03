# libroot

Library for dynamically obtaining the jailbreak-root and rootfs paths and converting paths around.
Part of "Rootless v2", an API that allows third party packages to run on any jailbreak, no matter where the jailbreaks bootstrapped is installed to.
Designed to be backwards compatible with all jailbreaks, even those that have not adopted the API.
Intended to be flexible enough to support undetectable jailbreaks with randomized paths, while providing a compatibility layer on other jailbreaks to avoid community fragmentation of third party packages (Like what was seem with "`iphoneos-arm64e`").

## Jailbreaks

Jailbreaks supporting rootless V2 are supposed to ship a package in their jailbreak that contains the `<JB_ROOT>/usr/lib/libroot.dylib` file.
This library is supposed to implement all of the following methods, which will be used by third party packages that access files.
If this file does exists, `libroot_dyn.a` will use a fallback implementation that uses `/` as the root path, `/var/jb` as the jailbreak root path on XinaA15 1.x and iphoneos-arm64 packages and `/` as the jailbreak root path in iphoneos-arm packages.

```c
const char *libroot_get_root_prefix(void);
```
This function should return a static buffer containing the prefix that the bootstrap of the jailbreak considers the rootfs, which is read-only and only contains the files shipped by the operating system. For most jailbreaks this will be "", the only exception is when a jailbreak uses something like scheme4 to make the bootstrap think it's in a different path. For example, RootHide uses `"/rootfs"` for this, as it's jailbreak considers that to correspond to the rootfs, so that `"/jbroot` can correspond to the jailbreak root.

```c
const char *libroot_get_jbroot_prefix(void);
```
This function should return a static buffer containing the prefix of the path where the bootstrap of the jailbreak is installed to, this path is read-write and contains all jailbreak related files. For most jailbreaks this will be "/var/jb", but if the jailbreak (like RootHide) wants to be undetectable, this needs to be a randomized path.

```c
const char *libroot_get_boot_uuid(void);
```
This function should return a static buffer containing a per boot UUID that identifies the current userspace boot. This UUID is expected to be 37 characters long, including the last byte being NULL. This merely exists for allowing third party packages to randomize certain tokens to avoid jailbreak detection. If your jailbreak is not supposed to be undetectable, you can return `"00000000-0000-0000-0000-000000000000"` here. A more detailed explanation can be found in the "How to Use" section.

```c
int libroot_convert_rootfs(const char *path, char *fullPathOut, size_t fullPathSize);
```
This function is supposed to provide the logic for converting a path to be a rootfs path. Sample input: `"/sbin/launchd"`, sample output: `"/sbin/launchd"` (sample output, RootHide: `/rootfs/sbin/launchd`).
Additionally it is supposed to provide logic for stripping the jbroot out of a jbroot path, if the path passed to it is inside that. Sample input: `/var/jb/Library/Application Support/CCSupport.bundle`, sample output: `/Library/Application Support/CCSupport.bundle`.

```c
int libroot_convert_jbroot(const char *path, char *fullPathOut, size_t fullPathSize);
```
This function is supposed to provide the logic for converting a path to be a rootfs path. Sample input: `"/Library/Application Support/CCSupport.bundle"`, sample output: `"/var/jb/Library/Application Support/CCSupport.bundle"`.

## How to use

First of all, link the corresponding .a file `libroot_dyn_iphoneos-arm64.a` or `libroot_dyn_iphoneos-arm.a` to your project (if you're using a recent theos version, this will be done automatically) and include the libroot.h header.
This header contains 3 macros to make it easy to interact with libroot:

```c
JBROOT_PATH(path)
```
This macro can be used to convert a path to the jailbreak root.

```c
ROOTFS_PATH(path)
```
This macro can be used to convert a path to what the bootstrap considers the rootfs (NOTE: This is only rarely neccessary if you need to interact with bootstrap binaries, for example if you want to make the bootstrap's `cp` binary copy a file from the rootfs to the jbroot, you would do so as follows: `JBROOT_PATH("/bin/sh") ROOTFS_PATH("/sbin/launchd") JBROOT_PATH("/sbin/launchd")`, if you want to access a file on the rootfs from your own binary, you do not want to use this macro). This macro can also be used to convert a jbroot path to a normal path (As explained under `libroot_convert_rootfs`).

```c
JBRAND
```
This macro is used to get a per userspace boot 37-char long UUID in the following format: `"00000000-0000-0000-0000-000000000000"`.
The main use case for this to avoid jailbreak detection of tweaks, one example would be when working with distributed notifications. Apps could listen for these to know when you're jailbroken, so you can prefix them with JBRAND to avoid this.