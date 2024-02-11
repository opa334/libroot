# libroot

Library for dynamically obtaining the jailbreak-root and rootfs paths and converting paths around.
Part of "Rootless v2", an API that allows third party packages to run on any jailbreak, no matter where the jailbreaks bootstrapped is installed to.
Designed to be backwards compatible with all jailbreaks, even those that have not adopted the API.
Intended to be flexible enough to support undetectable jailbreaks with randomized paths, while providing a compatibility layer on other jailbreaks to avoid community fragmentation of third party packages (Like what was seen with "`iphoneos-arm64e`").

## Adapting libroot in a jailbreak

### Providing paths

Jailbreaks supporting rootless V2 are supposed to ship a package in their jailbreak that contains the `<JB_ROOT>/usr/lib/libroot.dylib` file.
This library can implement the following functions, which will be used by third party packages that access files.
If this library does not exist, `libroot_dyn.a` will use a fallback implementation that will make callers use the following paths:
|               | iphoneos-arm64 | XinaA15 1.x    | iphoneos-arm   |
| ------------- | -------------- | -------------- | -------------- |
| rootfs        | `/`            | `/`            | `/`            |
| jbroot        | `/var/jb`      | `/var/jb`      | `/`            |

```c
const char *libroot_get_root_prefix(void);
```
This function shall return a static buffer containing the prefix that the bootstrap of the jailbreak considers the rootfs, which is read-only and only contains the files shipped by the operating system. For most jailbreaks this will be `""`, the only exception is when a jailbreak uses something like scheme4 to make the bootstrap think it's in a different path. For example, RootHide uses `"/rootfs"` for this, as it's bootstrap considers that to correspond to the rootfs, so that `"/jbroot"` can correspond to the jailbreak root.


```c
const char *libroot_get_jbroot_prefix(void);
```
This function shall return a static buffer containing the prefix of the path where the bootstrap of the jailbreak is installed to, this path is read-write and contains all jailbreak related files. For most jailbreaks this will be "/var/jb" or what that points to, but if the jailbreak (like RootHide) wants to be undetectable, this needs to be a randomized path.


```c
const char *libroot_get_boot_uuid(void);
```
This function shall return a static buffer containing a per boot UUID that identifies the current userspace boot. This UUID is expected to be 37 characters long, including the last byte being NULL. This merely exists for allowing third party packages to randomize certain tokens to avoid jailbreak detection. If your jailbreak is not supposed to be undetectable, you can return `"00000000-0000-0000-0000-000000000000"` here or avoid implementing it alltogether. A more detailed explanation can be found in the "How to Use" section.


### Custom path conversion logic

The libroot_dyn static library already provides logic for converting a relative path to a rootfs or jbroot path. If your jailbreak wants to provide custom conversion logic though, it can do so by implementing the following two functions.

```c
char *libroot_rootfspath(const char *path, char *resolvedPath);
```
This function is supposed to provide the logic for converting a path to be a rootfs path. Sample input: `"/sbin/launchd"`, sample output: `"/sbin/launchd"` (sample output, RootHide: `/rootfs/sbin/launchd`).
Additionally it is supposed to provide logic for stripping the jbroot out of a jbroot path, if the path passed to it is inside that. Sample input: `/var/jb/Library/Application Support/CCSupport.bundle`, sample output: `/Library/Application Support/CCSupport.bundle`. If `resolvedPath` is not NULL, it should be treated as a buffer of PATH_MAX characters and returned upon success, if it is NULL, the function is supposed to allocate a buffer of PATH_MAX bytes by itself. Unless you need custom logic, it is recommended to leave this function unimplemented.


```c
char *libroot_jbrootpath(const char *path, char *resolvedPath);
```
This function is supposed to provide the logic for converting a path to be a jbroot path. Sample input: `"/Library/Application Support/CCSupport.bundle"`, sample output: `"/var/jb/Library/Application Support/CCSupport.bundle"`. If `resolvedPath` is not NULL, it should be treated as a buffer of PATH_MAX characters and returned upon success, if it is NULL, the function is supposed to allocate a buffer of PATH_MAX bytes by itself.  Unless you need custom logic, it is recommended to leave this function unimplemented.

## Using libroot in a project

First of all, link the corresponding .a file `libroot_dyn_iphoneos-arm64.a` or `libroot_dyn_iphoneos-arm.a` to your project (if you're using a recent theos version, this will be done automatically) and include the libroot.h header.
This header contains 3 macros to make it easy to interact with libroot:


```c
JBROOT_PATH_CSTRING(path)
JBROOT_PATH_NSSTRING(path)
```
These macros can be used to convert a path to the jailbreak root.


```c
ROOTFS_PATH_CSTRING(path)
ROOTFS_PATH_NSSTRING(path)
```
These macros can be used to convert a path to what the bootstrap considers the rootfs (NOTE: This is only rarely neccessary if you need to interact with bootstrap binaries, for example if you want to make the bootstrap's `cp` binary copy a file from the rootfs to the jbroot, you would do so as follows: `JBROOT_PATH("/bin/sh") ROOTFS_PATH("/sbin/launchd") JBROOT_PATH("/sbin/launchd")`). If you want to access a file on the rootfs from the context of your own process, you do NOT want to use this macro. This macro can also be used to convert a jbroot path to a normal path (As explained under `libroot_jbrootpath`).

```c
JBRAND
```
This macro is used to get a per userspace boot 37-char long UUID in the following format: `"00000000-0000-0000-0000-000000000000"`, the main use case for this to avoid jailbreak detection of tweaks. One example would be when working with distributed notifications, apps could listen for these to know when you're jailbroken, so you can prefix them with JBRAND on both the sender and the receivers end to avoid this, as apps cannot predit this UUID.

Additionally it is also possible to call 
```c
char *libroot_dyn_rootfspath(const char *path, char *resolvedPath);
char *libroot_dyn_jbrootpath(const char *path, char *resolvedPath);
```
directly for more control over the arguments, the function signature is similar to [realpath](https://man7.org/linux/man-pages/man3/realpath.3.html).