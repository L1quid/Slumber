#ifndef _SLUMBER_DB_H
#define _SLUMBER_DB_H

#include "../yaml/include/yaml.h"
#include <string>
#include "ptrlist.h"
#include "assocarray.h"
#include "../../hightime.h"

struct TASKSTRUCT
{
  std::string title, command, params, last_run_at;
  int interval, interval_type;
  CHighTime last_run_at_timestamp;
};

#define MAP_TYPE TASKSTRUCT
#define MAP_LIST WDL_PtrList< MAP_TYPE >

namespace Slumber
{
  class DB
  {
  public:
    static int save_task_list(const char *filename, MAP_LIST *tasks);
    static MAP_LIST *get_task_list(const char *filename);
  };
}

#endif // _SLUMBER_DB_H