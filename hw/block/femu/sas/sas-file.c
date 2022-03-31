#include "sas-file.h"

static int sasFileClose(sqlite3_file*);
static int sasFileRead(sqlite3_file*, void*, int iAmt, sqlite3_int64 iOfst);
static int sasFileWrite(sqlite3_file*, const void*, int iAmt, sqlite3_int64 iOfst);
static int sasFileTruncate(sqlite3_file*, sqlite3_int64 size);
static int sasFileSync(sqlite3_file*, int flags);
static int sasFileFileSize(sqlite3_file*, sqlite3_int64 *pSize);
static int sasFileLock(sqlite3_file*, int);
static int sasFileUnlock(sqlite3_file*, int);
static int sasFileCheckReservedLock(sqlite3_file*, int *pResOut);
static int sasFileFileControl(sqlite3_file*, int op, void *pArg);
static int sasFileSectorSize(sqlite3_file*);
static int sasFileDeviceCharacteristics(sqlite3_file*);
