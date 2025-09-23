#ifndef OSMM_H
#define OSMM_H


#define MM_PAGING
#define PAGING_MAX_MMSWP 4 /* max number of supported swapped space */
#define PAGING_MAX_SYMTBL_SZ 30

typedef char BYTE;
typedef uint32_t addr_t;
//typedef unsigned int uint32_t;
// cấu trúc dịnh nghĩa số trang
struct pgn_t{
   int pgn; // Số trang (Page Number)
   struct pgn_t *pg_next; //trỏ đến trang kế tiếp 
};

/*
 *  Memory region struct : Vùng nhớ 
 */
struct vm_rg_struct {
   unsigned long rg_start; //địa chỉ bắt đầu
   unsigned long rg_end; //địa chỉ kết thúc

   struct vm_rg_struct *rg_next; //con trỏ đến vùng nhớ tiếp theo
};

/*
 *  Memory area struct : Vùng bộ nhớ của tiến trình
 */
struct vm_area_struct {
   unsigned long vm_id;  // ID của vùng bộ nhớ
   unsigned long vm_start;   // Địa chỉ bắt đầu vùng nhớ
   unsigned long vm_end;  // Địa chỉ kết thúc vùng nhớ

   unsigned long sbrk;   // Dùng để mở rộng heap (giống syscall `sbrk`)
/*
 * Derived field
 * unsigned long vm_limit = vm_end - vm_start
 */
   struct mm_struct *vm_mm;  // Trỏ đến cấu trúc quản lý bộ nhớ
   struct vm_rg_struct *vm_freerg_list;  // Danh sách vùng nhớ trống
   struct vm_area_struct *vm_next;    // Con trỏ đến vùng nhớ tiếp theo
};

/* 
 * Memory management struct :  Quản lý bộ nhớ của tiến trình
 */
struct mm_struct {
   uint32_t *pgd;   // Con trỏ đến bảng trang gốc (Page Directory)

   struct vm_area_struct *mmap;  // Danh sách các vùng bộ nhớ của tiến trình
 
   /* Currently we support a fixed number of symbol */
   struct vm_rg_struct symrgtbl[PAGING_MAX_SYMTBL_SZ];  //là bảng lưu trữ thông tin về các vùng nhớ đã cấp phát cho tiến trình, được đánh chỉ mục theo số hiệu thanh ghi (reg) mà tiến trình dùng để lưu địa chỉ.

   /* list of free page */
   struct pgn_t *fifo_pgn;   // Danh sách trang theo chiến lược thay thế FIFO
};

/*
 * FRAME/MEM PHY struct :   khung trang vật lý
 */
struct framephy_struct { 
   int fpn;// Số thứ tự của khung trang
   struct framephy_struct *fp_next; // Con trỏ đến khung kế tiếp

   /* Resereed for tracking allocated framed */
   struct mm_struct* owner; //Tiến trình đang sử dụng khung trang.
};

struct memphy_struct {  // Quản lý bộ nhớ vật lý
   /* Basic field of data and size */
   BYTE *storage;     // Bộ nhớ vật lý
   int maxsz;  // Kích thước tối đa của bộ nhớ
   
   /* Sequential device fields */ 
   int rdmflg;
   int cursor;

   /* Management structure */
   struct framephy_struct *free_fp_list; // frame trống
   struct framephy_struct *used_fp_list;
};

#endif
/*
mm_struct (bộ nhớ của tiến trình)
│
├── mmap (Danh sách vùng nhớ ảo - VMA)
│   ├── vm_area_struct (Vùng nhớ 1)
│   │   ├── vm_freerg_list (Danh sách vùng trống trong vùng nhớ này)
│   │   │   ├── vm_rg_struct (Vùng trống 1)
│   │   │   ├── vm_rg_struct (Vùng trống 2)
│   │   │   └── ...
│   │   └── vm_next → vm_area_struct (Vùng nhớ 2)
│   ├── vm_area_struct (Vùng nhớ 2)
│   └── ...
│
├── symrgtbl[] (Bảng biểu tượng: thông tin các vùng đã cấp phát theo biến/rgid )
│   ├── vm_rg_struct (vùng của biến reg0)
│   ├── vm_rg_struct (vùng của biến reg1)
│   └── ...
│
├── pgd[] (Page Table: ánh xạ page number → frame number trong RAM)
│   ├── PTE[0] → Frame 1
│   ├── PTE[1] → Frame 4
│   └── ...
│
└── fifo_pgn (Danh sách các page đã dùng gần nhất – phục vụ page replacement FIFO)
Thành phần	Mô tả
RAM	Được mô phỏng bằng 1 memphy_struct với rdmflg = 1
SWAP	Là một memphy_struct khác, rdmflg = 0, truy cập tuần tự
Frame allocation	Duyệt free_fp_list, cấp phát các fpn
I/O thực tế	read() / write() tương tác với storage[]

*/