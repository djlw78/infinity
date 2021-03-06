/* Copyright (C) 2015 - GruntTheDivine (Sloan Crandell)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/*
 * pipe.c
 * Provides functions for creating and dealing with pipes.
 */

#include <stddef.h>
#include <infinity/kernel.h>
#include <infinity/dirent.h>
#include <infinity/heap.h>
#include <infinity/fs.h>
#include <infinity/sched.h>
#include <infinity/fifobuf.h>
#include <infinity/fcntl.h>
#include <infinity/sync.h>

#define PIPE_BUFFER_SIZE   4096 * 4


static void pipe_writec(struct pipe_buf *pipe, char c);
static char pipe_readc(struct pipe_buf *pipe);
static int pipe_write(struct file *fd, const char *buf, off_t off, size_t len);
static int pipe_read(struct file *fd, char *buf, off_t off, size_t len);
static int pipe_fstat(struct file *f, struct stat *st);
static int pipe_close(struct fildes *fd);

/*
 * Creates a unidirectional pipe with f[1] being the
 * input for f[0]. 
 * @param f     An array of struct file pointers, this 
 *              should be empty.
 */
int pipe(int *pipedes)
{
    struct file *files[2];
    int res = fpipe(files);
    if(res == 0) {
        pipedes[0] = create_fildes("anon_pipe", O_RDONLY, files[0]);
        pipedes[1] = create_fildes("anon_pipe", O_WRONLY, files[1]);
        printk(KERN_INFO "pipe() fildes are %d, %d\n", pipedes[0], pipedes[1]);
        return 0;
    }
    return -1;
}
 
int fpipe(struct file *f[])
{
    struct file *f1 = kalloc(sizeof(struct file));
    struct file *f2 = kalloc(sizeof(struct file));
    memset(f1, 0, sizeof(struct file));
    memset(f2, 0, sizeof(struct file));
    f1->read = pipe_read;
    f2->write = pipe_write;
    f1->fstat = pipe_fstat;
    f2->fstat = pipe_fstat;
    f1->close = pipe_close;
    f2->close = pipe_close;
    struct fifo_buffer *buf = (struct fifo_buffer*)kalloc(sizeof(struct fifo_buffer));
    fifo_init(buf, PIPE_BUFFER_SIZE);
    f1->f_tag = buf;
    f2->f_tag = buf;
    add_to_file_table(f1);
    add_to_file_table(f2);
    f[0] = f1;
    f[1] = f2;
    return 0;
}

/*
 * Blocks the thread until there is enough room to
 * fit into buf, then proceeds to write the data
 */
static int pipe_write(struct file *fd, const char *buf, off_t off, size_t len)
{
    struct fifo_buffer *pipe = fd->f_tag;
    int r = fifo_write(pipe, buf, len);
    return r;
}

/*
 * Blocks the thread until there the buffer has 
 * some data in it, then reads it into buf
 */
static int pipe_read(struct file *fd, char *buf, off_t off, size_t len)
{
    struct fifo_buffer *pipe = fd->f_tag;
    int r = fifo_read(pipe, buf, len);
    return r;
}

static int pipe_fstat(struct file *f, struct stat *st)
{
    return 0;
}

static int pipe_close(struct fildes *fd)
{
    struct fifo_buffer *pipe = fd->fd_file->f_tag;
    if(fd->fd_file->f_refs == 0 && (fd->fd_mode & O_WRONLY)) {
        pipe->f_brk = 1;
    }
    return 0;
}
