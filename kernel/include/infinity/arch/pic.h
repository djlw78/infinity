/* Copyright (C) 2014 - GruntTheDivine (Sloan Crandell)
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

#ifndef INFINITY_INTERRUPT_H
#define INFINITY_INTERRUPT_H

struct regs {
    uint32_t    ds;
    uint32_t    edi;
    uint32_t    esi;
    uint32_t    ebp;
    uint32_t    esp;
    uint32_t    ebx;
    uint32_t    edx;
    uint32_t    ecx;
    uint32_t    eax;
    uint32_t    interrupt;
    uint32_t    err_code;
    uint32_t    eip;
    uint32_t    cs;
    uint32_t    eflags;
    uint32_t    useresp;
    uint32_t    ss;
};


typedef void (*inthandler_t)(struct regs *registers);

int request_isr(int inum, inthandler_t handler);
int free_isr(int inum);

int request_irq(int inum, inthandler_t handler);
int free_irq(int inum);

#endif
