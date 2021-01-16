#ifndef COMMON_PROCESS_INFO_PROCESS_INFO_H_
#define COMMON_PROCESS_INFO_PROCESS_INFO_H_

#include <stdio.h>
#include <stdlib.h>

typedef enum process_type_enum {
    PROCESS_TYPE_MASTER = 0,
    PROCESS_TYPE_LOGGER = 1,
    PROCESS_TYPE_WORKER = 2
} process_type_t;

typedef struct process_info_struct {
    process_type_t type;
    pid_t pid;
    struct process_info_struct **childs;
    int childs_max_count;
    int childs_count;
} process_info_t;

process_info_t *process_info_init(pid_t pid, process_type_t type, int childs_max_count);
void process_info_free(process_info_t *process_info);
int process_info_add_child(process_info_t *parent, process_info_t *child);

#endif  // COMMON_PROCESS_INFO_PROCESS_INFO_H_
