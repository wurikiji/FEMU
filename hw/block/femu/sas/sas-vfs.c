#include "sas_vfs.h"
#include "sqlite3.h"

#define SAS_VFS_NAME "sas-vfs"
#define MIN_BYTE_TO_WRITE 512

typedef struct sas_file sas_file;
struct sas_file {
  sqlite3_file base;
};

static int sasVfsOpen(sqlite3_vfs *, const char *, sqlite3_file *, int, int *);
static int sasVfsDelete(sqlite3_vfs *, const char *zName, int syncDir);
static int sasVfsAccess(sqlite3_vfs *, const char *zName, int flags, int *);
static int sasVfsFullPathname(sqlite3_vfs *, const char *zName, int nOut,
                              char *zOut);
static void *sasVfsDlOpen(sqlite3_vfs *, const char *zFilename);
static void sasVfsDlError(sqlite3_vfs *, int nByte, char *zErrMsg);
static void (*sasVfsDlSym(sqlite3_vfs *, void *, const char *zSymbol))(void);
static void sasVfsDlClose(sqlite3_vfs *, void *);
static int sasVfsRandomness(sqlite3_vfs *, int nByte, char *zOut);
static int sasVfsSleep(sqlite3_vfs *, int microseconds);
static int sasVfsCurrentTime(sqlite3_vfs *, double *);

static sas_vfs sas_fs = {
    .base =
        {
            .iVersion = 1,
            .szOsFile = 0,
            .mxPathname = 0,
            .pNext = 0,
            .zName = SAS_VFS_NAME,
            .pAppData = 0,
            .xOpen = sasVfsOpen,
            .xDelete = sasVfsDelete,
            .xAccess = sasVfsAccess,
            .xFullPathname = sasVfsFullPathname,
            .xDlOpen = sasVfsDlOpen,
            .xDlError = sasVfsDlError,
            .xDlSym = sasVfsDlSym,
            .xDlClose = sasVfsDlClose,
            .xRandomness = sasVfsRandomness,
            .xSleep = sasVfsSleep,
            .xCurrentTime = sasVfsCurrentTime,
        },
};

static int sasVfsOpen(sqlite3_vfs *pVfs, const char *zName, sqlite3_file *pFile,
                      int flags, int *pOutFlags) {
  sas_vfs *pSasVfs = (sas_vfs *)pVfs;
}
static int sasVfsDelete(sqlite3_vfs *pVfs, const char *zName, int syncDir) {}
static int sasVfsAccess(sqlite3_vfs *pVfs, const char *zName, int flags, int *pResOut) {}
static int sasVfsFullPathname(sqlite3_vfs *pVfs, const char *zName, int nOut,
                              char *zOut) {}
static void *sasVfsDlOpen(sqlite3_vfs *pVfs, const char *zFilename) {}
static void sasVfsDlError(sqlite3_vfs *pVfs, int nByte, char *zErrMsg) {}
static void (*sasVfsDlSym(sqlite3_vfs *pVfs, void *pHandle, const char *zSymbol))(void) {}
static void sasVfsDlClose(sqlite3_vfs *pVfs, void *pHandle) {}
static int sasVfsRandomness(sqlite3_vfs *pVfs, int nByte, char *zOut) {}
static int sasVfsSleep(sqlite3_vfs *pVfs, int microseconds) {}
static int sasVfsCurrentTime(sqlite3_vfs *pVfs, double *pTimeout) {
}

int sas_vfs_register(void) {
}