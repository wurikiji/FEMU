#ifndef SAS_VFS_H
#define SAS_VFS_H 1

#include "sqlite3.h"
#include "sas-file.h"

#define SAS_VFS_NAME "sas-vfs"
#define MIN_BYTE_TO_WRITE 512

typedef struct sas_vfs sas_vfs;

struct sas_vfs {
  sqlite3_vfs base;
  sqlite3_vfs *pParent;
  sas_real_file *pFileList;
};
int sasVfsOpen(sqlite3_vfs *, const char *, sqlite3_file *, int, int *);
int sasVfsDelete(sqlite3_vfs *, const char *zName, int syncDir);
int sasVfsAccess(sqlite3_vfs *, const char *zName, int flags, int *);
int sasVfsFullPathname(sqlite3_vfs *, const char *zName, int nOut,
                              char *zOut);
void *sasVfsDlOpen(sqlite3_vfs *, const char *zFilename);
void sasVfsDlError(sqlite3_vfs *, int nByte, char *zErrMsg);
void (*sasVfsDlSym(sqlite3_vfs *, void *, const char *zSymbol))(void);
void sasVfsDlClose(sqlite3_vfs *, void *);
int sasVfsRandomness(sqlite3_vfs *, int nByte, char *zOut);
int sasVfsSleep(sqlite3_vfs *, int microseconds);
int sasVfsCurrentTime(sqlite3_vfs *, double *);

sas_vfs sas_fs = {
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
#endif