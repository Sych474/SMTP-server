#include "process_info.h"

process_info_t *process_info_init(pid_t pid, process_type_t type, int childs_max_count) {
    process_info_t *process_info = malloc(sizeof(process_info_t));
    if (!process_info)
        return NULL;

    process_info->childs = malloc(sizeof(process_info_t *) * childs_max_count);
    if (!process_info->childs) {
        free(process_info);
        return NULL;
    }

    for (size_t i = 0; i < childs_max_count; i++)
        process_info->childs[i] = NULL;

    process_info->childs_max_count = childs_max_count;
    process_info->childs_count = 0;
    process_info->type = type;
    process_info->pid = pid;
    return process_info;
}

void process_info_free(process_info_t *process_info) {
    if (process_info) {
        for (size_t i = 0; i < process_info->childs_max_count; i++)
            process_info_free(process_info->childs[i]);

        free(process_info->childs);
        free(process_info);
    }
}

int process_info_add_child(process_info_t *parent, process_info_t *child) {
    if (parent->childs_count == parent->childs_max_count)
        return -1;
    if (parent->childs[parent->childs_count] != NULL)
        return -2;

    parent->childs[parent->childs_count] = child;
    parent->childs_count++;
    return 0;
}
