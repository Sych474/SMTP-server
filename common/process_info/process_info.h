#ifndef COMMON_PROCESS_INFO_PROCESS_INFO_H_
#define COMMON_PROCESS_INFO_PROCESS_INFO_H_

typedef enum process_type_enum {
    PROCESS_TYPE_MASTER = 0,
    PROCESS_TYPE_LOGGER = 1,
    PROCESS_TYPE_WORKER = 2
} process_type_t;

typedef struct process_info_struct {
    process_type_t process_type;
} process_info_t;

#endif  // COMMON_PROCESS_INFO_PROCESS_INFO_H_
