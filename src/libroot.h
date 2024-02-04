const char *libroot_dyn_get_root_prefix(void);
const char *libroot_dyn_get_jbroot_prefix(void);
const char *libroot_dyn_get_boot_uuid(void);

int libroot_dyn_convert_rootfs(const char *path, char *fullPathOut, size_t fullPathSize);
int libroot_dyn_convert_jbroot(const char *path, char *fullPathOut, size_t fullPathSize);

#ifdef __OBJC__

#define __CONVERT_PATH_NSSTRING(converter, path) ({ \
	char tmpBuf[PATH_MAX]; \
	converter(path.fileSystemRepresentation, tmpBuf, PATH_MAX); \
	[NSString stringWithUTF8String:tmpBuf]; \
})

#define JBROOT_PATH_NSSTRING(path) __CONVERT_PATH_NSSTRING(libroot_dyn_convert_jbroot, path)
#define ROOTFS_PATH_NSSTRING(path) __CONVERT_PATH_NSSTRING(libroot_dyn_convert_rootfs, path)

#endif /* __OBJC__ */

#define __CONVERT_PATH_CSTRING(converter, path) ({ \
	static char outPath[PATH_MAX]; \
	converter(path, outPath, PATH_MAX); \
	outPath; \
})

#define JBROOT_PATH_CSTRING(path) __CONVERT_PATH_CSTRING(libroot_dyn_convert_jbroot, path)
#define ROOTFS_PATH_CSTRING(path) __CONVERT_PATH_CSTRING(libroot_dyn_convert_rootfs, path)

#define JBRAND libroot_dyn_get_boot_uuid()
