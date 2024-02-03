
const char *libroot_dyn_get_root_prefix(void);
const char *libroot_dyn_get_jbroot_prefix(void);
const char *libroot_dyn_get_boot_uuid(void);

int libroot_dyn_convert_rootfs(const char *path, char *fullPathOut, size_t fullPathSize);
int libroot_dyn_convert_jbroot(const char *path, char *fullPathOut, size_t fullPathSize);

#define __CONVERT_PATH_NSSTRING(converter, path) ({ \
	char tmpBuf[PATH_MAX]; \
	converter(path.fileSystemRepresentation, tmpBuf, PATH_MAX); \
	[NSString stringWithUTF8String:tmpBuf]; \
})

#define __CONVERT_PATH_C(converter, path) ({ \
	static char outPath[PATH_MAX]; \
	converter(path, outPath, PATH_MAX); \
	outPath; \
})

#define __JBROOT_PATH_NSSTRING(path) __CONVERT_PATH_NSSTRING(libroot_dyn_convert_jbroot, path)
#define __JBROOT_PATH_C(path) __CONVERT_PATH_C(libroot_dyn_convert_jbroot, path)

#define JBROOT_PATH(path) _Generic((path), \
    NSString *:   __JBROOT_PATH_NSSTRING(path), \
	char *:       __JBROOT_PATH_C(path), \
	const char *: __JBROOT_PATH_C(path), \
)

#define __ROOTFS_PATH_NSSTRING(path) __CONVERT_PATH_NSSTRING(libroot_dyn_convert_rootfs, path)
#define __ROOTFS_PATH_C(path) __CONVERT_PATH_C(libroot_dyn_convert_rootfs, path)

#define ROOTFS_PATH(path) _Generic((path), \
	NSString *:   __ROOTFS_PATH_NSSTRING(path), \
	char *:       __ROOTFS_PATH_C(path), \
	const char *: __ROOTFS_PATH_C(path), \
)

#define JBRAND libroot_dyn_get_boot_uuid()