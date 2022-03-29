#ifndef SAS_VFS_H
#define SAS_VFS_H

#include "sqlite3.h"

typedef struct sas_vfs sas_vfs;
struct sas_vfs {
  sqlite3_vfs base;
  sqlite3_vfs *pParent;
};
#endif SAS_VFS_H