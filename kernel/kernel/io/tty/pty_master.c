/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

#include <fs/vfs.h>
#include <io/tty/pty_master.h>
#include <io/tty/pty_slave.h>
#include <libkern/bits/errno.h>
#include <libkern/libkern.h>
#include <libkern/log.h>

#define INODE2PTSNO(x) (x - 1)
#define PTSNO2INODE(x) (x + 1)

pty_master_entry_t pty_masters[PTYS_COUNT];

int _pty_master_free_dentry_data(dentry_t* dentry);
bool pty_master_can_read(dentry_t* dentry, uint32_t start);
bool pty_master_can_write(dentry_t* dentry, uint32_t start);
int pty_master_read(dentry_t* dentry, uint8_t* buf, uint32_t start, uint32_t len);
int pty_master_write(dentry_t* dentry, uint8_t* buf, uint32_t start, uint32_t len);
int pty_master_fstat(dentry_t* dentry, fstat_t* stat);

static fs_ops_t pty_master_ops = {
    .recognize = 0,
    .prepare_fs = 0,
    .eject_device = 0,
    .dentry = {
        .read_inode = 0,
        .write_inode = 0,
        .free_inode = _pty_master_free_dentry_data,
        .get_fsdata = 0,
    },
    .file = {
        .can_read = pty_master_can_read,
        .can_write = pty_master_can_write,
        .read = pty_master_read,
        .write = pty_master_write,
        .open = 0,
        .truncate = 0,
        .create = 0,
        .unlink = 0,
        .getdents = 0,
        .lookup = 0,
        .mkdir = 0,
        .rmdir = 0,
        .fstat = pty_master_fstat,
        .ioctl = 0,
        .mmap = 0,
    }
};

static pty_master_entry_t* _ptm_get(dentry_t* dentry)
{
    for (int i = 0; i < PTYS_COUNT; i++) {
        if (dentry == &pty_masters[i].dentry) {
            return &pty_masters[i];
        }
    }
    return NULL;
}

int _pty_master_free_dentry_data(dentry_t* dentry)
{
    pty_master_entry_t* ptm = _ptm_get(dentry);
    ASSERT(ptm);
    ptm->dentry.inode_indx = 0;
    return 0;
}

bool pty_master_can_read(dentry_t* dentry, uint32_t start)
{
    pty_master_entry_t* ptm = _ptm_get(dentry);
    ASSERT(ptm);
    return sync_ringbuffer_space_to_read(&ptm->buffer) >= 1;
}

bool pty_master_can_write(dentry_t* dentry, uint32_t start)
{
    pty_master_entry_t* ptm = _ptm_get(dentry);
    ASSERT(ptm);
    return sync_ringbuffer_space_to_write(&ptm->pts->buffer) >= 0;
}
