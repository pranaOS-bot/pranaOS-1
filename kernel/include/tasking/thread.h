/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _KERNEL_TASKING_THREAD_H
#define _KERNEL_TASKING_THREAD_H

#include <drivers/generic/fpu.h>
#include <fs/vfs.h>
#include <libkern/lock.h>
#include <libkern/types.h>
#include <platform/generic/tasking/context.h>
#include <platform/generic/tasking/trapframe.h>
#include <tasking/signal.h>
#include <time/time_manager.h>

enum THREAD_STATUS {
    THREAD_INVALID = 0,
    THREAD_ALLOCATED,
    THREAD_RUNNING,
    THREAD_DEAD,
    THREAD_STOPPED,
    THREAD_BLOCKED,
    THREAD_DYING,
};

struct thread;
struct blocker {
    int reason;
    int (*should_unblock)(struct thread* p);
    bool should_unblock_for_signal;
};
typedef struct blocker blocker_t;

enum BLOCKER_REASON {
    BLOCKER_INVALID,
    BLOCKER_JOIN,
    BLOCKER_READ,
    BLOCKER_WRITE,
    BLOCKER_SLEEP,
    BLOCKER_SELECT,
    BLOCKER_DUMPING,
};

struct proc;
struct thread {
    struct proc* process;
    uint32_t tid;
    uint32_t status;

    /* Kernel data */
    zone_t kstack;
    context_t* context; // context of kernel's registers
    trapframe_t* tf;
    fpu_state_t* fpu_state;

    /* Scheduler data */
    struct thread* sched_prev;
    struct thread* sched_next;
    int last_cpu;
    time_t ticks_until_preemption;
    time_t start_time_in_ticks; // Time when the task was put to run.

    /* Blocker data */
    blocker_t blocker;
    int exit_code;
    struct thread* joinee;
    file_descriptor_t* blocker_fd;
    time_t unblock_time;
    int nfds;
    fd_set_t readfds;
    fd_set_t writefds;
    fd_set_t exceptfds;

    /* Stat data */
    time_t stat_total_running_ticks;

    uint32_t signals_mask;
    uint32_t pending_signals_mask;
    void* signal_handlers[SIGNALS_CNT];
};
typedef struct thread thread_t;

#define THREADS_PER_NODE (128)
struct thread_list_node {
    thread_t thread_storage[THREADS_PER_NODE];
    struct thread_list_node* next;
    int empty_spots;
};
typedef struct thread_list_node thread_list_node_t;

struct thread_list {
    struct thread_list_node* head;
    struct thread_list_node* next_empty_node;
    int next_empty_index;
    lock_t lock;
    struct thread_list_node* tail;
};
typedef struct thread_list thread_list_t;

/**
 * THREAD FUNCTIONS
 */

int thread_setup_main(struct proc* p, thread_t* thread);
int thread_setup(struct proc* p, thread_t* thread);
int thread_setup_kstack(thread_t* thread);
int thread_copy_of(thread_t* thread, thread_t* from_thread);

int thread_fill_up_stack(thread_t* thread, int argc, char** argv, char** env);

int thread_kstack_free(thread_t* thread);
int thread_free(thread_t* thread);
int thread_die(thread_t* thread);

static ALWAYS_INLINE int thread_is_free(thread_t* thread) { return (thread->status == THREAD_INVALID) || (thread->status == THREAD_DEAD); }

/**
 * BLOCKER FUNCTIONS
 */

int init_join_blocker(thread_t* p);
int init_read_blocker(thread_t* p, file_descriptor_t* bfd);
int init_write_blocker(thread_t* thread, file_descriptor_t* bfd);
int init_sleep_blocker(thread_t* thread, uint32_t time);
int init_select_blocker(thread_t* thread, int nfds, fd_set_t* readfds, fd_set_t* writefds, fd_set_t* exceptfds, timeval_t* timeout);

/**
 * DEBUG FUNCTIONS
 */

int thread_dump_frame(thread_t* thread);
int thread_print_backtrace();

#endif /* _KERNEL_TASKING_THREAD_H */