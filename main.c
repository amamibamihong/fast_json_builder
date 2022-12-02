//
// Created by zhangshang on 2022/12/2.
//
#include "json_util.h"

int main(int argc, char **argv, char **envp) {
    json_builder *json_builder = json_builder_new();
    if (json_builder) {
        json_builder_open_object(json_builder);
        json_builder_object_add_string(json_builder,"str","value");
        json_builder_object_add_integer(json_builder,"int",123);
        json_builder_object_add_real(json_builder,"real",123.123);
        json_builder_close(json_builder);
    }
    printf("json: %s\n", json_builder_get_str(json_builder));
    json_builder_destroy(json_builder);
}
