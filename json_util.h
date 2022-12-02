//
// Created by zhangshang on 2022/12/1.
//

#ifndef _JSON_UTIL_H
#define _JSON_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "strbuffer.h"


typedef struct _json_builder {
  uint16_t type;
  uint16_t state;
  strbuffer_t strbuffer;
} json_builder;

json_builder *json_builder_new(void);

int json_builder_open_object(json_builder *jb);

int json_builder_open_array(json_builder *jb);

int json_builder_object_add_string(json_builder *jb, const char *key, const char *value);

int json_builder_object_add_integer(json_builder *jb, const char *key, int64_t value);

int json_builder_object_add_real(json_builder *jb, const char *key, double value);

int json_builder_object_add_object(json_builder *jb, const char *key, json_builder *value);

int json_builder_object_add_true(json_builder *jb, const char *key);

int json_builder_object_add_false(json_builder *jb, const char *key);

int json_builder_object_add_null(json_builder *jb, const char *key);

int json_builder_array_add_string(json_builder *jb, const char *value);

int json_builder_array_add_integer(json_builder *jb, int64_t value);

int json_builder_array_add_real(json_builder *jb, double value);

int json_builder_array_add_object(json_builder *jb, json_builder *value);

int json_builder_array_add_true(json_builder *jb);

int json_builder_array_add_false(json_builder *jb);

int json_builder_array_add_null(json_builder *jb);

int json_builder_close(json_builder *jb);

void json_builder_destroy(json_builder *jb);

void json_builder_clear(json_builder *jb);

const char *json_builder_get_str(json_builder *jb);

#endif //_JSON_UTIL_H
