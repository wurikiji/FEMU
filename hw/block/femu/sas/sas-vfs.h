#ifndef SAS_VFS_H
#define SAS_VFS_H 1

#include "sqlite3.h"

#define SAS_VFS_NAME "sas-vfs"
#define SAS_SECTOR_SIZE 512
#define SAS_PAGE_SIZE 4096

typedef struct sas_real_file sas_real_file;
struct sas_real_file {
  sqlite3_file *pFile;
  const char *zName;
  int size;
  sas_real_file *pNext;
  sas_real_file **ppThis;
  char *data;
};

typedef struct sas_file sas_file;
struct sas_file {
  sqlite3_file base;
  sas_real_file *pReal;
};

int sasFileClose(sqlite3_file *);
int sasFileRead(sqlite3_file *, void *, int iAmt, sqlite3_int64 iOfst);
int sasFileWrite(sqlite3_file *, const void *, int iAmt, sqlite3_int64 iOfst);
int sasFileTruncate(sqlite3_file *, sqlite3_int64 size);
int sasFileSync(sqlite3_file *, int flags);
int sasFileFileSize(sqlite3_file *, sqlite3_int64 *pSize);
int sasFileLock(sqlite3_file *, int);
int sasFileUnlock(sqlite3_file *, int);
int sasFileCheckReservedLock(sqlite3_file *, int *pResOut);
int sasFileFileControl(sqlite3_file *, int op, void *pArg);
int sasFileSectorSize(sqlite3_file *);
int sasFileDeviceCharacteristics(sqlite3_file *);

sqlite3_io_methods sas_io_methods = {
    .iVersion = 1,
    .xClose = sasFileClose,
    .xRead = sasFileRead,
    .xWrite = sasFileWrite,
    .xTruncate = sasFileTruncate,
    .xSync = sasFileSync,
    .xFileSize = sasFileFileSize,
    .xLock = sasFileLock,
    .xUnlock = sasFileUnlock,
    .xCheckReservedLock = sasFileCheckReservedLock,
    .xFileControl = sasFileFileControl,
    .xSectorSize = sasFileSectorSize,
    .xDeviceCharacteristics = sasFileDeviceCharacteristics,
};

typedef struct sas_vfs sas_vfs;

struct sas_vfs {
  sqlite3_vfs base;
  sas_real_file *pFileList;
};
int sasVfsOpen(sqlite3_vfs *, const char *, sqlite3_file *, int, int *);
int sasVfsDelete(sqlite3_vfs *, const char *zName, int syncDir);
int sasVfsAccess(sqlite3_vfs *, const char *zName, int flags, int *);
int sasVfsFullPathname(sqlite3_vfs *, const char *zName, int nOut, char *zOut);
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
    .pFileList = 0,
};
#endif