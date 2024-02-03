#include <stdint.h>
#include <stddef.h>
#include <dispatch/dispatch.h>
#include <dlfcn.h>

#ifdef IPHONEOS_ARM64

const char *libroot_fallback_get_root_prefix(void)
{
	return "";
}

const char *libroot_fallback_get_jbroot_prefix(void)
{
	return "/var/jb";
}

#else

const char *libroot_fallback_get_root_prefix(void)
{
	return "";
}

const char *libroot_fallback_get_jbroot_prefix(void)
{
	if (access("/var/LIY", F_OK) == 0) {
		// Legacy support for XinaA15 1.x (For those two people still using it)
		// Technically this should be deprecated, but with the libroot solution it's not the hardest thing in the world to maintain
		// So I decided to leave it in
		return "/var/jb";
	}
	else {
		return "";
	}
}

#endif

const char *libroot_fallback_get_boot_uuid(void)
{
	return "00000000-0000-0000-0000-000000000000";
}

int libroot_fallback_convert_rootfs(const char *path, char *fullPathOut, size_t fullPathSize)
{
	if (!fullPathOut || fullPathSize == 0) return -1;
	strlcpy(fullPathOut, libroot_fallback_get_root_prefix(), fullPathSize);
	strlcat(fullPathOut, path, fullPathSize);
	return 0;
}

int libroot_fallback_convert_jbroot(const char *path, char *fullPathOut, size_t fullPathSize)
{
	if (!fullPathOut || fullPathSize == 0) return -1;
	strlcpy(fullPathOut, libroot_fallback_get_jbroot_prefix(), fullPathSize);
	strlcat(fullPathOut, path, fullPathSize);
	return 0;
}

static const char *(*dyn_get_root_prefix)(void);
static const char *(*dyn_get_jbroot_prefix)(void);
static const char *(*dyn_get_boot_uuid)(void);
static int (*dyn_convert_rootfs)(const char *path, char *fullPathOut, size_t fullPathSize);
static int (*dyn_convert_jbroot)(const char *path, char *fullPathOut, size_t fullPathSize);

void libroot_load(void)
{
	static dispatch_once_t onceToken;
	dispatch_once (&onceToken, ^{
		void *handle = dlopen("@rpath/usr/lib/libroot.dylib", RTLD_NOW);
		if (handle) {
			dyn_get_root_prefix   = dlsym(handle, "libroot_get_root_prefix");
			dyn_get_jbroot_prefix = dlsym(handle, "libroot_get_jbroot_prefix");
			dyn_get_boot_uuid     = dlsym(handle, "libroot_get_boot_uuid");
			dyn_convert_rootfs    = dlsym(handle, "libroot_convert_rootfs");
			dyn_convert_jbroot    = dlsym(handle, "libroot_convert_jbroot");
		}
		else {
			dyn_get_root_prefix   = libroot_fallback_get_root_prefix;
			dyn_get_jbroot_prefix = libroot_fallback_get_jbroot_prefix;
			dyn_get_boot_uuid     = libroot_fallback_get_boot_uuid;
			dyn_convert_rootfs    = libroot_fallback_convert_rootfs;
			dyn_convert_jbroot    = libroot_fallback_convert_jbroot;
		}
	});
}

const char *libroot_dyn_get_root_prefix(void)
{
	libroot_load();
	return dyn_get_root_prefix();
}

const char *libroot_dyn_get_jbroot_prefix(void)
{
	libroot_load();
	return dyn_get_jbroot_prefix();
}

const char *libroot_dyn_get_boot_uuid(void)
{
	libroot_load();
	return dyn_get_boot_uuid();
}

int libroot_dyn_convert_rootfs(const char *path, char *fullPathOut, size_t fullPathSize)
{
	libroot_load();
	return dyn_convert_rootfs(path, fullPathOut, fullPathSize);
}

int libroot_dyn_convert_jbroot(const char *path, char *fullPathOut, size_t fullPathSize)
{
	libroot_load();
	return dyn_convert_jbroot(path, fullPathOut, fullPathSize);
}