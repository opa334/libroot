#include <sys/syslimits.h>

__BEGIN_DECLS

const char *_Nonnull libroot_dyn_get_root_prefix(void);
const char *_Nonnull libroot_dyn_get_jbroot_prefix(void);
const char *_Nonnull libroot_dyn_get_boot_uuid(void);
char *_Nullable libroot_dyn_rootfspath(const char *_Nullable path, char *_Nullable resolvedPath);
char *_Nullable libroot_dyn_jbrootpath(const char *_Nullable path, char *_Nullable resolvedPath);

__END_DECLS

#define __CONVERT_PATH_CSTRING(converter, path) ({ \
	static char outPath[PATH_MAX]; \
	converter(path, outPath); \
})

#define JBROOT_PATH_CSTRING(path) __CONVERT_PATH_CSTRING(libroot_dyn_jbrootpath, path)
#define ROOTFS_PATH_CSTRING(path) __CONVERT_PATH_CSTRING(libroot_dyn_rootfspath, path)

#if __has_attribute(overloadable)
// the C version needs to be inlined because we cannot re-use the static buffer
__attribute__((__overloadable__, __always_inline__))
static inline const char *__libroot_convert_path(char *(*converter)(const char *, char *), const char *path) {
	return __CONVERT_PATH_CSTRING(converter, path);
}
#endif /* __has_attribute(overloadable) */

#ifdef __OBJC__

#define __CONVERT_PATH_NSSTRING(converter, path) ({ \
	char tmpBuf[PATH_MAX]; \
	[NSString stringWithUTF8String:converter(path.fileSystemRepresentation, tmpBuf)]; \
})

#define JBROOT_PATH_NSSTRING(path) __CONVERT_PATH_NSSTRING(libroot_dyn_jbrootpath, path)
#define ROOTFS_PATH_NSSTRING(path) __CONVERT_PATH_NSSTRING(libroot_dyn_rootfspath, path)

#if __has_attribute(overloadable)
__attribute__((__overloadable__))
static inline NSString *const __libroot_convert_path(char *(*converter)(const char *, char *), NSString *path) {
	return __CONVERT_PATH_NSSTRING(converter, path);
}
#endif /* __has_attribute(overloadable) */

#endif /* __OBJC__ */

#if __has_attribute(overloadable)
#	define JBROOT_PATH(path) __libroot_convert_path(libroot_dyn_jbrootpath, path)
#	define ROOTFS_PATH(path) __libroot_convert_path(libroot_dyn_rootfspath, path)
#else
#	define JBROOT_PATH(path) _Pragma("GCC error \"JBROOT_PATH is not supported with this compiler, use JBROOT_PATH_CSTRING or JBROOT_PATH_NSSTRING\"") path
#	define ROOTFS_PATH(path) _Pragma("GCC error \"ROOTFS_PATH is not supported with this compiler, use ROOTFS_PATH_CSTRING or ROOTFS_PATH_NSSTRING\"") path
#endif /* __has_attribute(overloadable) */

#define JBRAND libroot_dyn_get_boot_uuid()
