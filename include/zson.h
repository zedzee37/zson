#ifndef ZSON_H
#define ZSON_H

#include "smap.h"

struct JsonElement {
    void *data;
    struct StrHashMap map;    
};

#endif
