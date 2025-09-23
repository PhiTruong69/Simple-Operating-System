#ifndef OSCFG_H
#define OSCFG_H

#define MLQ_SCHED 1 // Bật chế độ lập lịch đa hàng đợi (Multi-Level Queue)
#define MAX_PRIO 140  // Độ ưu tiên tối đa của tiến trình

#define MM_PAGING // Bật chế độ quản lý bộ nhớ phân trang
// #define MM_FIXED_MEMSZ     // (Tắt) Quản lý bộ nhớ với vùng nhớ cố định, không dùng khi đã bật PAGING
// #define VMDBG 1             // (Tắt) Debug VM (Virtual Memory)
// #define MMDBG 1            // (Tắt) Debug bộ nhớ nói chung
#define IODUMP 1   // Bật in log I/O (đọc/ghi bộ nhớ ảo)
#define PAGETBL_DUMP 1     // Bật in bảng trang

#endif
