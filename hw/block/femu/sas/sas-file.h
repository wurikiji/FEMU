#ifndef SAS_FILE_H
#define SAS_FILE_H 1

#include "sqlite3.h"
typedef struct sas_real_file sas_real_file;
struct sas_real_file {
  sqlite3_file *pFile;
  const char *zName;
  int dbSize;
  int jnrSize;  // if this file is journal file then this will be set to non 0
  int blobSize;
  int refCount;
  sas_real_file *pNext;
  sas_real_file **ppThis;
};

typedef struct sas_file sas_file;
struct sas_file {
  sqlite3_file base;
  int eType;
  sas_real_file *pReal;
};

int sasFileClose(sqlite3_file *);
int sasFileRead(sqlite3_file *, void *, int iAmt, sqlite3_int64 iOfst);
int sasFileWrite(sqlite3_file *, const void *, int iAmt,
                        sqlite3_int64 iOfst);
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
#endif