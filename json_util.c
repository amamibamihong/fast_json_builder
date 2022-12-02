//
// Created by zhangshang on 2022/12/1.
//

#include "json_util.h"

typedef enum _jb_type {
  Object,
  Array,
} jb_type;
typedef enum _jb_state {
  None = 0,
  ObjectFirst,
  ObjectNth,
  ArrayFirst,
  ArrayNth,
} jb_state;

//#define INIT_STR_BUFFER_LEN 512
#define MAX_INTEGER_STR_LENGTH 100
#define MAX_REAL_STR_LENGTH    100
#define JSON_INTEGER_FORMAT "lld"

static int dtostr(char *buffer, size_t size, double value, int precision);


json_builder *json_builder_new(void)
{
  json_builder *jb = calloc(1, sizeof(json_builder));
  if (jb == NULL) {
    return NULL;
  }
  if (strbuffer_init(&jb->strbuffer)) {
    free(jb);
    return NULL;
  }
  return jb;
}

int json_builder_open_object(json_builder *jb)
{

  if (strbuffer_append_byte(&jb->strbuffer, '{')) {
    return -1;
  }
  jb->type = Object;
  jb->state = ObjectFirst;
  return 0;
}

int json_builder_open_array(json_builder *jb)
{

  if (strbuffer_append_byte(&jb->strbuffer, '[')) {
    return -1;
  }
  jb->type = Array;
  jb->state = ArrayFirst;
  return 0;
}

int json_builder_object_add_string(json_builder *jb, const char *key, const char *value)
{
  uint16_t rst_state = jb->state;
  size_t rst_len = jb->strbuffer.length;
  if (jb->state == ObjectNth) {
    if (strbuffer_append_bytes(&jb->strbuffer, ",\"", 2)) {
      goto reset;
    }
  } else {
    if (strbuffer_append_byte(&jb->strbuffer, '"')) {
      goto reset;
    }
    jb->state = ObjectNth;
  }
  if (strbuffer_append_str(&jb->strbuffer, key)) {
    goto reset;
  }
  if (strbuffer_append_str(&jb->strbuffer, "\":\"")) {
    goto reset;
  }
  if (strbuffer_append_str(&jb->strbuffer, value)) {
    goto reset;
  }
  if (strbuffer_append_byte(&jb->strbuffer, '"')) {
    goto reset;
  }
  return 0;
  reset: // 恢复到之前状态
  jb->state = rst_state;
  jb->strbuffer.length = rst_len;
  jb->strbuffer.value[rst_len] = '\0';
  return -1;
}

int json_builder_object_add_integer(json_builder *jb, const char *key, int64_t value)
{
  uint16_t rst_state = jb->state;
  size_t rst_len = jb->strbuffer.length;
  if (jb->state == ObjectNth) {
    if (strbuffer_append_str(&jb->strbuffer, ",\"")) {
      goto reset;
    }
  } else {
    if (strbuffer_append_byte(&jb->strbuffer, '"')) {
      goto reset;
    }
    jb->state = ObjectNth;
  }
  if (strbuffer_append_str(&jb->strbuffer, key)) {
    goto reset;
  }
  if (strbuffer_append_str(&jb->strbuffer, "\":")) {
    goto reset;
  }
  char buffer[MAX_INTEGER_STR_LENGTH];
  int size;

  size = snprintf(buffer, MAX_INTEGER_STR_LENGTH, "%" JSON_INTEGER_FORMAT, value);
  if (size < 0 || size >= MAX_INTEGER_STR_LENGTH)
    goto reset;

  if (strbuffer_append_str(&jb->strbuffer, buffer)) {
    goto reset;
  }
  return 0;
  reset:
  jb->state = rst_state;
  jb->strbuffer.length = rst_len;
  jb->strbuffer.value[rst_len] = '\0';
  return -1;
}

int json_builder_object_add_real(json_builder *jb, const char *key, double value)
{
  uint16_t rst_state = jb->state;
  size_t rst_len = jb->strbuffer.length;
  if (jb->state == ObjectNth) {
    if (strbuffer_append_str(&jb->strbuffer, ",\"")) {
      goto reset;
    }
  } else {
    if (strbuffer_append_byte(&jb->strbuffer, '"')) {
      goto reset;
    }
    jb->state = ObjectNth;
  }
  if (strbuffer_append_str(&jb->strbuffer, key)) {
    goto reset;
  }
  if (strbuffer_append_str(&jb->strbuffer, "\":")) {
    goto reset;
  }
  char buffer[MAX_REAL_STR_LENGTH];
  int size;

  size = dtostr(buffer, MAX_REAL_STR_LENGTH, value, 0);
  if (size < 0)
    goto reset;

  if (strbuffer_append_str(&jb->strbuffer, buffer)) {
    goto reset;
  }
  return 0;
  reset:
  jb->state = rst_state;
  jb->strbuffer.length = rst_len;
  jb->strbuffer.value[rst_len] = '\0';
  return -1;
}


int json_builder_object_add_object(json_builder *jb, const char *key, json_builder *value)
{
  uint16_t rst_state = jb->state;
  size_t rst_len = jb->strbuffer.length;
  if (value == NULL) {
    return 0;
  }
  if (jb->state == ObjectNth) {
    if (strbuffer_append_str(&jb->strbuffer, ",\"")) {
      goto reset;
    }
  } else {
    if (strbuffer_append_byte(&jb->strbuffer, '"')) {
      goto reset;
    }
    jb->state = ObjectNth;
  }

  if (strbuffer_append_str(&jb->strbuffer, key)) {
    goto reset;
  }
  if (strbuffer_append_str(&jb->strbuffer, "\":")) {
    goto reset;
  }
  if (strbuffer_append_str(&jb->strbuffer, value->strbuffer.value)) {
    goto reset;
  }
  return 0;
  reset:
  jb->state = rst_state;
  jb->strbuffer.length = rst_len;
  jb->strbuffer.value[rst_len] = '\0';
  return -1;
}

int json_builder_object_add_true(json_builder *jb, const char *key)
{
  uint16_t rst_state = jb->state;
  size_t rst_len = jb->strbuffer.length;


  if (jb->state == ObjectNth) {
    if (strbuffer_append_str(&jb->strbuffer, ",\"")) {
      goto reset;
    }
  } else {
    if (strbuffer_append_byte(&jb->strbuffer, '"')) {
      goto reset;
    }
    jb->state = ObjectNth;
  }
  if (strbuffer_append_str(&jb->strbuffer, key)) {
    goto reset;
  }
  if (strbuffer_append_str(&jb->strbuffer, "\":true")) {
    goto reset;
  }
  return 0;
  reset:
  jb->state = rst_state;
  jb->strbuffer.length = rst_len;
  jb->strbuffer.value[rst_len] = '\0';
  return -1;

}

int json_builder_object_add_false(json_builder *jb, const char *key)
{
  uint16_t rst_state = jb->state;
  size_t rst_len = jb->strbuffer.length;
  if (jb->state == ObjectNth) {
    if (strbuffer_append_str(&jb->strbuffer, ",\"")) {
      goto reset;
    }
  } else {
    if (strbuffer_append_byte(&jb->strbuffer, '"')) {
      goto reset;
    }
    jb->state = ObjectNth;
  }
  if (strbuffer_append_str(&jb->strbuffer, key)) {
    goto reset;
  }
  if (strbuffer_append_str(&jb->strbuffer, "\":false")) {
    goto reset;
  }
  return 0;
  reset:
  jb->state = rst_state;
  jb->strbuffer.length = rst_len;
  jb->strbuffer.value[rst_len] = '\0';
  return -1;
}

int json_builder_object_add_null(json_builder *jb, const char *key)
{
  uint16_t rst_state = jb->state;
  size_t rst_len = jb->strbuffer.length;
  if (jb->state == ObjectNth) {
    if (strbuffer_append_str(&jb->strbuffer, ",\"")) {
      goto reset;
    }
  } else {
    if (strbuffer_append_byte(&jb->strbuffer, '"')) {
      goto reset;
    }
    jb->state = ObjectNth;
  }
  if (strbuffer_append_str(&jb->strbuffer, key)) {
    goto reset;
  }
  if (strbuffer_append_str(&jb->strbuffer, "\":null")) {
    goto reset;
  }
  return 0;
  reset:
  jb->state = rst_state;
  jb->strbuffer.length = rst_len;
  jb->strbuffer.value[rst_len] = '\0';
  return -1;
}


int json_builder_array_add_string(json_builder *jb, const char *value)
{
  uint16_t rst_state = jb->state;
  size_t rst_len = jb->strbuffer.length;
  if (jb->state == ArrayNth) {
    if (strbuffer_append_bytes(&jb->strbuffer, ",\"", 2)) {
      goto reset;
    }
  } else {
    if (strbuffer_append_byte(&jb->strbuffer, '"')) {
      goto reset;
    }
    jb->state = ArrayNth;
  }
  if (strbuffer_append_str(&jb->strbuffer, value)) {
    goto reset;
  }
  if (strbuffer_append_byte(&jb->strbuffer, '"')) {
    goto reset;
  }
  return 0;
  reset:
  jb->state = rst_state;
  jb->strbuffer.length = rst_len;
  jb->strbuffer.value[rst_len] = '\0';
  return -1;
}

int json_builder_array_add_integer(json_builder *jb, int64_t value)
{
  uint16_t rst_state = jb->state;
  size_t rst_len = jb->strbuffer.length;
  if (jb->state == ArrayNth) {
    if (strbuffer_append_byte(&jb->strbuffer, ',')) {
      goto reset;
    }
  } else {
    jb->state = ArrayNth;
  }
  char buffer[MAX_INTEGER_STR_LENGTH];
  int size;
  size = snprintf(buffer, MAX_INTEGER_STR_LENGTH, "%" JSON_INTEGER_FORMAT, value);
  if (size < 0 || size >= MAX_INTEGER_STR_LENGTH)
    goto reset;
  if (strbuffer_append_str(&jb->strbuffer, buffer)) {
    goto reset;
  }
  return 0;
  reset:
  jb->state = rst_state;
  jb->strbuffer.length = rst_len;
  jb->strbuffer.value[rst_len] = '\0';
  return -1;
}

int json_builder_array_add_real(json_builder *jb, double value)
{
  uint16_t rst_state = jb->state;
  size_t rst_len = jb->strbuffer.length;
  if (jb->state == ArrayNth) {
    if (strbuffer_append_byte(&jb->strbuffer, ',')) {
      goto reset;
    }
  } else {
    jb->state = ArrayNth;
  }
  char buffer[MAX_REAL_STR_LENGTH];
  int size;

  size = dtostr(buffer, MAX_REAL_STR_LENGTH, value, 0);
  if (size < 0)
    goto reset;

  if (strbuffer_append_str(&jb->strbuffer, buffer)) {
    goto reset;
  }
  return 0;
  reset:
  jb->state = rst_state;
  jb->strbuffer.length = rst_len;
  jb->strbuffer.value[rst_len] = '\0';
  return -1;
}


int json_builder_array_add_object(json_builder *jb, json_builder *value)
{
  uint16_t rst_state = jb->state;
  size_t rst_len = jb->strbuffer.length;
  if (value == NULL) {
    return 0;
  }
  if (jb->state == ArrayNth) {
    if (strbuffer_append_byte(&jb->strbuffer, ',')) {
      goto reset;
    }
  } else {
    jb->state = ArrayNth;
  }
  if (strbuffer_append_str(&jb->strbuffer, value->strbuffer.value)) {
    goto reset;
  }
  return 0;
  reset:
  jb->state = rst_state;
  jb->strbuffer.length = rst_len;
  jb->strbuffer.value[rst_len] = '\0';
  return -1;
}

int json_builder_array_add_true(json_builder *jb)
{
  if (jb->state == ArrayNth) {
    if (strbuffer_append_str(&jb->strbuffer, ",true")) {
      goto reset;
    }
  } else {
    if (strbuffer_append_str(&jb->strbuffer, "true")) {
      goto reset;
    }
    jb->state = ArrayNth;
  }
  return 0;
  reset:
  return -1;
}

int json_builder_array_add_false(json_builder *jb)
{
  if (jb->state == ArrayNth) {
    if (strbuffer_append_str(&jb->strbuffer, ",false")) {
      goto reset;
    }
  } else {
    if (strbuffer_append_str(&jb->strbuffer, "false")) {
      goto reset;
    }
    jb->state = ArrayNth;
  }
  return 0;
  reset:
  return -1;
}

int json_builder_array_add_null(json_builder *jb)
{
  if (jb->state == ArrayNth) {
    if (strbuffer_append_str(&jb->strbuffer, ",null")) {
      goto reset;
    }
  } else {
    if (strbuffer_append_str(&jb->strbuffer, "null")) {
      goto reset;
    }
    jb->state = ArrayNth;
  }
  return 0;
  reset:
  return -1;
}

int json_builder_close(json_builder *jb)
{
  if (jb->type == Object) {
    if (strbuffer_append_str(&jb->strbuffer, "}")) {
      goto error;
    }
  } else {
    if (strbuffer_append_str(&jb->strbuffer, "]")) {
      goto error;
    }
  }
  return 0;
  error:
  // 内存不足关闭失败清空消息
  strbuffer_clear(&jb->strbuffer);
  return -1;
}

void json_builder_destroy(json_builder *jb)
{
  if (jb == NULL) {
    return;
  }
  strbuffer_close(&jb->strbuffer);
  free(jb);
}

void json_builder_clear(json_builder *jb)
{
  jb->state = None;
  strbuffer_clear(&jb->strbuffer);
}

const char *json_builder_get_str(json_builder *jb)
{
  return strbuffer_value(&jb->strbuffer);
}

static int dtostr(char *buffer, size_t size, double value, int precision)
{
  int ret;
  char *start, *end;
  size_t length;

  if (precision == 0)
    precision = 17;

  ret = snprintf(buffer, size, "%.*g", precision, value);
  if (ret < 0)
    return -1;

  length = (size_t) ret;
  if (length >= size)
    return -1;
  /* Make sure there's a dot or 'e' in the output. Otherwise
     a real is converted to an integer when decoding */
  if (strchr(buffer, '.') == NULL && strchr(buffer, 'e') == NULL) {
    if (length + 3 >= size) {
      /* No space to append ".0" */
      return -1;
    }
    buffer[length] = '.';
    buffer[length + 1] = '0';
    buffer[length + 2] = '\0';
    length += 2;
  }

  /* Remove leading '+' from positive exponent. Also remove leading
     zeros from exponents (added by some printf() implementations) */
  start = strchr(buffer, 'e');
  if (start) {
    start++;
    end = start + 1;

    if (*start == '-')
      start++;

    while (*end == '0')
      end++;

    if (end != start) {
      memmove(start, end, length - (size_t) (end - buffer));
      length -= (size_t) (end - start);
    }
  }

  return (int) length;
}