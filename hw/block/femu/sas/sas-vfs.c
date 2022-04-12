#include "sas-vfs.h"

#include <stdint.h>
#include <string.h>
#include <time.h>

#include "sqlite3.h"
// sas io methods
int sasFileClose(sqlite3_file* pFile) {
  // TODO: Flush buffer
}

int sasFileRead(sqlite3_file* pFile, void* buffer, int iAmt,
                sqlite3_int64 iOfst) {
  int rc = SQLITE_OK;
  sas_file* p = (sas_file*)pFile;
  sas_real_file* pReal = p->pReal;
  if (pReal->size <= iOfst + iAmt) {
    return SQLITE_IOERR_READ;
  }
  memcpy(buffer, pReal->data + iOfst, iAmt);
  return rc;
}

int sasFileWrite(sqlite3_file* pFile, const void* buffer, int iAmt,
                 sqlite3_int64 iOfst) {
  int rc = SQLITE_OK;
  sas_file* p = (sas_file*)pFile;
  sas_real_file* pReal = p->pReal;
  sqlite3_file* pF = pReal->pFile;

  if (pReal->size <= iOfst + iAmt) {
    pReal->data = (void*)sqlite3_realloc(pReal->data, iOfst + iAmt);
  }

  memcpy(pReal->data + iOfst, buffer, iAmt);
  // TODO: NAND OPeration
  return rc;
}

int sasFileTruncate(sqlite3_file* pFile, sqlite3_int64 size) {
  sas_file* p = (sas_file*)pFile;
  sas_real_file* pReal = p->pReal;
  // TODO: DISCARD truncated area
  pReal->size = pReal->size < size ? pReal->size : size;
  return SQLITE_OK;
}
int sasFileSync(sqlite3_file* pFile, int flags) {
  sas_file* p = (sas_file*)pFile;
  sas_real_file* pReal = p->pReal;
  sqlite3_file* pRealFile = pReal->pFile;
  int rc = SQLITE_OK;

  // TODO: keep db size on reboot

  return rc;
}
int sasFileFileSize(sqlite3_file* pFile, sqlite3_int64* pSize) {
  sas_file* p = (sas_file*)pFile;
  sas_real_file* pReal = p->pReal;
  *pSize = pReal->size;
  return SQLITE_OK;
}
int sasFileLock(sqlite3_file* pFile, int flag) { return SQLITE_OK; }
int sasFileUnlock(sqlite3_file* pFile, int flag) { return SQLITE_OK; }
int sasFileCheckReservedLock(sqlite3_file* pFile, int* pResOut) {
  *pResOut = 0;
  return SQLITE_OK;
}
int sasFileFileControl(sqlite3_file* pFile, int op, void* pArg) {
  if (op == SQLITE_FCNTL_PRAGMA) return SQLITE_NOTFOUND;
  return SQLITE_OK;
}
int sasFileSectorSize(sqlite3_file* pFile) { return SAS_SECTOR_SIZE; }
int sasFileDeviceCharacteristics(sqlite3_file* pFile) { return 0; }




// sas vfs methods
// DISCUSSION: do we need to simplify this for SaS?
// Or, are there any possibility to support multi database situation?
int sasVfsOpen(sqlite3_vfs *pVfs, const char *zName, sqlite3_file *pFile,
               int flags, int *pOutFlags) {
  sas_vfs *pSasVfs = (sas_vfs *)pVfs;
  sas_file *pF = (sas_file *)pFile;
  sas_real_file *pReal = NULL;
  int lenFilename;
  int rc = SQLITE_OK;

  pF->base.pMethods = &sas_io_methods;

  lenFilename = (int)strlen(zName);
  pReal = pSasVfs->pFileList;
  for (; pReal != NULL && strncmp(pReal->zName, zName, lenFilename);
       pReal = pReal->pNext)
    ;
  if (pReal != NULL) {
    pF->pReal = pReal;
  } else {
    int real_flag = (flags & ~(SQLITE_OPEN_MAIN_DB)) | SQLITE_OPEN_TEMP_DB;
    sqlite3_int64 size;

    // create real file
    pReal = (sas_real_file *)sqlite3_malloc(sizeof(sas_real_file));
    if (pReal == NULL) {
      return SQLITE_NOMEM;
    }
    memset(pReal, 0, sizeof(sas_real_file));

    // open file
    pReal->zName = (char *)sqlite3_malloc(lenFilename);
    memcpy(pReal->zName, zName, lenFilename);
    pReal->pFile = pF;
    // TODO: Allocate FTL based memory offset
    // TODO: when the file was already created before.
    // manage file list
    if (rc == SQLITE_OK) {
      pReal->pNext = pSasVfs->pFileList;
      if (pReal->pNext != NULL) {
        pReal->pNext->ppThis = &pReal->pNext;
      }
      pReal->ppThis = &pSasVfs->pFileList;
      pSasVfs->pFileList = pReal;
    }
  }
  return rc;
}

// TODO: Invalidate FTL MAP
// TODO: Invalidate memory map
int sasVfsDelete(sqlite3_vfs *pVfs, const char *zName, int syncDir) {
  // In FEMU mode, data will be written to memory,
  // so deletion need to wipe data from memory.
  // But this function should handle FTL's invalidation routine.
  sas_vfs *pSasVfs = (sas_vfs *)pVfs;
  sas_real_file *pReal;
  sqlite3_file *pF;
  int length = (int)strlen(zName);

  pReal = pSasVfs->pFileList;
  for (; pReal && strncmp(pReal->zName, zName, length); pReal = pReal->pNext)
    ;
  if (pReal) {
    // TODO: invalidate FTL map
    pReal->size = 0;
  }
  return SQLITE_OK;
}

int sasVfsAccess(sqlite3_vfs *pVfs, const char *zName, int flags,
                 int *pResOut) {
  sas_vfs *pSasVfs = (sas_vfs *)pVfs;
  sas_real_file *pReal;
  int length = (int)strlen(zName);

  // if file exist, then it can be accessed
  pReal = pSasVfs->pFileList;
  for (; pReal && strncmp(pReal->zName, zName, length); pReal = pReal->pNext)
    ;
  *pResOut = (pReal != NULL);
  return SQLITE_OK;
}

// No need to calculate absolute path
int sasVfsFullPathname(sqlite3_vfs *pVfs, const char *zName, int nOut,
                       char *zOut) {
  sqlite3_snprintf(nOut, zOut, "%s", zName);
  zOut[nOut - 1] = '\0';
  return SQLITE_OK;
}
void *sasVfsDlOpen(sqlite3_vfs *pVfs, const char *zFilename) { return 0; }
void sasVfsDlError(sqlite3_vfs *pVfs, int nByte, char *zErrMsg) {
  sqlite3_snprintf(nByte, zErrMsg, "Loadable extensions are not supported");
  zErrMsg[nByte - 1] = '\0';
}
void (*sasVfsDlSym(sqlite3_vfs *pVfs, void *pHandle,
                   const char *zSymbol))(void) {
  return 0;
}
void sasVfsDlClose(sqlite3_vfs *pVfs, void *pHandle) { return; }
int sasVfsRandomness(sqlite3_vfs *pVfs, int nByte, char *zOut) {
  return SQLITE_OK;
}
int sasVfsSleep(sqlite3_vfs *pVfs, int microseconds) {
  sleep(microseconds / 1000000);
  usleep(microseconds % 1000000);
  return microseconds;
}
int sasVfsCurrentTime(sqlite3_vfs *pVfs, double *pTime) {
  time_t t = time(0);
  *pTime = t / 86400.0 + 2440587.5;
  return SQLITE_OK;
}

int sas_vfs_register(void) {
  sas_fs.base.mxPathname = 18;
  sas_fs.base.szOsFile = sizeof(sas_file);
  return sqlite3_vfs_register((sqlite3_vfs *)&sas_fs, 0);
}