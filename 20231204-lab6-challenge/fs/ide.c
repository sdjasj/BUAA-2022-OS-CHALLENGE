/*
 * operations on IDE disk.
 */

#include "fs.h"
#include "lib.h"
#include <mmu.h>

// Overview:
// 	read data from IDE disk. First issue a read request through
// 	disk register and then copy data from disk buffer
// 	(512 bytes, a sector) to destination array.
//
// Parameters:
//	diskno: disk number.
// 	secno: start sector number.
// 	dst: destination for data read from IDE disk.
// 	nsecs: the number of sectors to read.
//
// Post-Condition:
// 	If error occurrs during the read of the IDE disk, panic.
//
// Hint: use syscalls to access device registers and buffers
/*** exercise 5.2 ***/
void
ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs)
{
	// 0x200: the size of a sector: 512 bytes.
	int offset_begin = secno * 0x200;
	int offset_end = offset_begin + nsecs * 0x200;
	int offset = 0;
	u_int devId = 0x13000010;
	u_int devOffset = 0x13000000;
	u_int start = 0x13000020;
	u_int ret = 0x13000030;
	u_int tmp = 0;
	while (offset_begin + offset < offset_end) {
		// Your code here
		// error occurred, then panic.
		int r = 0;
		tmp = 0;
		r = syscall_write_dev(&tmp, devId, 4);
		if (r) user_panic("ide_read ID  error\n");
		tmp = offset_begin + offset;
		r = syscall_write_dev(&tmp, devOffset, 4);
		if (r) user_panic("ide_read offset error\n");
		tmp = 0;
		r = syscall_write_dev(&tmp, start, 4);
		if (r) user_panic("ide_read start error\n");
		r = syscall_read_dev(&tmp, ret, 4);
		if (r != 0 || tmp == 0) user_panic("ide_read return of sys_read  error!!!!!!\n");
		r = syscall_read_dev(dst + offset, 0x13004000, 512);
		if (r) user_panic("ide_read real error!!!!!!\n");
		offset += 0x200;
	}
}


// Overview:
// 	write data to IDE disk.
//
// Parameters:
//	diskno: disk number.
//	secno: start sector number.
// 	src: the source data to write into IDE disk.
//	nsecs: the number of sectors to write.
//
// Post-Condition:
//	If error occurrs during the read of the IDE disk, panic.
//
// Hint: use syscalls to access device registers and buffers
/*** exercise 5.2 ***/
void
ide_write(u_int diskno, u_int secno, void *src, u_int nsecs)
{
	// Your code here
	int offset_begin = secno * 0x200;
    int offset_end = offset_begin + nsecs * 0x200;
    int offset = 0;
    u_int devId = 0x13000010;
    u_int devOffset = 0x13000000;
    u_int start = 0x13000020;
    u_int ret = 0x13000030;
	
	// DO NOT DELETE WRITEF !!!
	writef("diskno: %d\n", diskno);

	u_int tmp = 0;
	while (offset_begin + offset < offset_end) {
		// copy data from source array to disk buffer.
        int r = 0;
		tmp = 0;
        r = syscall_write_dev(src + offset, 0x13004000, 512);
        if (r) user_panic("ide_write write error!!!!!!\n");
        r = syscall_write_dev(&tmp, devId, 4);
        if (r) user_panic("ide_write ID  error\n");
        tmp = offset_begin + offset;
        r = syscall_write_dev(&tmp, devOffset, 4);
        if (r) user_panic("ide_write offset error\n");
        tmp = 1;
        r = syscall_write_dev(&tmp, start, 4);
		if (r) user_panic("ide_write start error\n");
		r = syscall_read_dev(&tmp, ret, 4);
        if (r != 0 || tmp == 0) user_panic("ide_wirte return of sys_write  error!!!!!!\n");
		offset += 0x200;
		// if error occur, then panic.
	}
}
/*
void
ide_write(u_int diskno, u_int secno, void *src, u_int nsecs)
{
        // Your code here
	int offset_begin = secno * 0x200;
	int offset_end = offset_begin + nsecs * 0x200;
	int offset = 0;

	int tmp;

	writef("diskno: %d\n", diskno);
	while (offset_begin + offset < offset_end) {
	    // copy data from source array to disk buffer.
		if (syscall_write_dev(src + offset, 0x13004000, 512)) user_panic("Error occurred during write the IDE disk!\n");
		tmp = 0;
		if (syscall_write_dev(&tmp, 0x13000010, 4)) user_panic("Error occurred during write the IDE disk!\n");
		tmp = offset_begin + offset;
		if (syscall_write_dev(&tmp, 0x13000000, 4)) user_panic("Error occurred during write the IDE disk!\n");
		tmp = 1;
		if (syscall_write_dev(&tmp, 0x13000020, 4)) user_panic("Error occurred during write the IDE disk!\n");
		if (syscall_read_dev(&tmp, 0x13000030, 4)) user_panic("Error occurred during write the IDE disk!\n");
		if (tmp == 0) user_panic("Error occurred during write the IDE disk!\n");
		offset += 0x200;
            // if error occur, then panic.
	}
}
*/
