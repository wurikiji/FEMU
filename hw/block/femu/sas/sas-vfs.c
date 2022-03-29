#include "sqlite3.h"

#define SAS_VFS_NAME "sas-vfs"
#define MIN_BYTE_TO_WRITE 512

typedef struct sas_vfs sas_vfs;
struct sas_vfs {
  sqlite3_vfs base;
};