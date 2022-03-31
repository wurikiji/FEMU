#include "sas-vfs.h"

#include <stdint.h>
#include <string.h>

#include "sas-file.h"
#include "sqlite3.h"

int sasVfsOpen(sqlite3_vfs *pVfs, const char *zName, sqlite3_file *pFile,
               int flags, int *pOutFlags) {
  sas_vfs *pSasVfs = (sas_vfs *)pVfs;
  sas_file *pF = (sas_file *)pFile;
  sas_real_file *pReal = NULL;
  int length;
  int eType = (flags & (SQLITE_OPEN_MAIN_DB | SQLITE_OPEN_MAIN_JOURNAL));
  int rc = SQLITE_OK;

  if (0 == eType) {
    // TODO: Temp file
  }

  pF->base.pMethods = &sas_io_methods;
  pF->eType = eType;

  length = (int)strlen(zName);
  pReal = pSasVfs->pFileList;
  for (; pReal && strncmp(pReal->zName, zName, length); pReal = pReal->pNext)
    ;
  if (pReal) {
    pF->pReal = pReal;
    pReal->refCount++;
  } else {
    int real_flag = (flags & ~(SQLITE_OPEN_MAIN_DB)) | SQLITE_OPEN_TEMP_DB;
    sqlite3_int64 size;
    sqlite3_file *pRealFile;
    sqlite3_vfs *pParent = pSasVfs->pParent;
    pReal = (sas_real_file *)sqlite3_malloc(sizeof(*pReal) + pParent->szOsFile);
    if (!pReal) {
      rc = SQLITE_NOMEM;
      sqlite3_free(pReal);
      return rc;
    }
    pRealFile = pReal->pFile;
    rc = pRealFile->pMethods->xFileSize(pRealFile, &size);
    if (rc != SQLITE_OK) {
      pRealFile->pMethods->xClose(pRealFile);
      sqlite3_free(pReal);
      return rc;
    }

    if (size == 0) {
      // When the file is empty, should we init ?
    } else {
      // if the file was already exist,
      // then get its size information
      uint32_t zS = 0;
      pReal->blobSize = (int)size;
      rc = pRealFile->pMethods->xRead(pRealFile, &zS, 4, 0);
      pReal->dbSize = zS;
      if (rc == SQLITE_OK) {
        rc = pRealFile->pMethods->xRead(pRealFile, &zS, 4, pReal->blobSize - 4);
        if (zS) {
          pReal->jnrSize = pReal->blobSize;
        }
      }
    }
    if (rc == SQLITE_OK) {
      pReal->pNext = pSasVfs->pFileList;
      if (pReal->pNext) {
        pReal->pNext->ppThis = &pReal->pNext;
      }
      pReal->ppThis = &pSasVfs->pFileList;
      pSasVfs->pFileList = pReal;
    }
  }
  return rc;
}

int sasVfsDelete(sqlite3_vfs *pVfs, const char *zName, int syncDir) {
  sqlite3_vfs *pParent = ((sas_vfs *)pVfs)->pParent;
  // In FEMU mode, data will be written to memory,
  // so deletion does not need to wipe data from memory.
  // But this function should handle FTL's invalidation routine.
  int rc = SQLITE_OK;
  sas_vfs *pSasVfs = (sas_vfs *)pVfs;
  sas_real_file *pReal;
  sqlite3_file *pF;
  int length = (int)strlen(zName);

  pReal = pSasVfs->pFileList;
  for (; pReal && strncmp(pReal->zName, zName, length); pReal = pReal->pNext)
    ;
  if (pReal) {
    pF = pReal->pFile;
    // TODO: invalidate FTL map
    rc = SQLITE_OK;
    // If it's journal, then remove
    pReal->jnrSize = 0;
    // TODO: remove from the list
  }
  return rc;
}

// this function is left for the comparison between off, rbj and wal
int sasVfsAccess(sqlite3_vfs *pVfs, const char *zName, int flags,
                 int *pResOut) {
  sas_vfs *pSasVfs = (sas_vfs *)pVfs;
  sas_real_file *pReal;
  int isJournal = 0;
  int length = (int)strlen(zName);

  if (length > 8 && strcmp("-journal", &zName[length - 8]) == 0) {
    isJournal = 1;
  }

  pReal = pSasVfs->pFileList;
  for (; pReal && strncmp(pReal->zName, zName, length); pReal = pReal->pNext)
    ;
  *pResOut = (pReal && (!isJournal || pReal->jnrSize > 0));
  return SQLITE_OK;
}
int sasVfsFullPathname(sqlite3_vfs *pVfs, const char *zName, int nOut,
                       char *zOut) {
  sqlite3_vfs *pParent = ((sas_vfs *)pVfs)->pParent;
  return pParent->xFullPathname(pParent, zName, nOut, zOut);
}
void *sasVfsDlOpen(sqlite3_vfs *pVfs, const char *zFilename) {
  sqlite3_vfs *pParent = ((sas_vfs *)pVfs)->pParent;
  return pParent->xDlOpen(pVfs, zFilename);
}
void sasVfsDlError(sqlite3_vfs *pVfs, int nByte, char *zErrMsg) {
  sqlite3_vfs *pParent = ((sas_vfs *)pVfs)->pParent;
  return pParent->xDlError(pVfs, nByte, zErrMsg);
}
void (*sasVfsDlSym(sqlite3_vfs *pVfs, void *pHandle,
                   const char *zSymbol))(void) {
  sqlite3_vfs *pParent = ((sas_vfs *)pVfs)->pParent;
  return pParent->xDlSym(pParent, pHandle, zSymbol);
}
void sasVfsDlClose(sqlite3_vfs *pVfs, void *pHandle) {
  sqlite3_vfs *pParent = ((sas_vfs *)pVfs)->pParent;
  return pParent->xDlClose(pParent, pHandle);
}
int sasVfsRandomness(sqlite3_vfs *pVfs, int nByte, char *zOut) {
  sqlite3_vfs *pParent = ((sas_vfs *)pVfs)->pParent;
  return pParent->xRandomness(pParent, nByte, zOut);
}
int sasVfsSleep(sqlite3_vfs *pVfs, int microseconds) {
  sqlite3_vfs *pParent = ((sas_vfs *)pVfs)->pParent;
  return pParent->xSleep(pParent, microseconds);
}
int sasVfsCurrentTime(sqlite3_vfs *pVfs, double *pTimeout) {
  sqlite3_vfs *pParent = ((sas_vfs *)pVfs)->pParent;
  return pParent->xCurrentTime(pParent, pTimeout);
}

int sas_vfs_register(void) {
  if (sas_fs.pParent) return SQLITE_OK;
  sas_fs.pParent = sqlite3_vfs_find(0);
  sas_fs.base.mxPathname = sas_fs.pParent->mxPathname;
  sas_fs.base.szOsFile = 4096;
  return sqlite3_vfs_register((sqlite3_vfs *)&sas_fs, 0);
}