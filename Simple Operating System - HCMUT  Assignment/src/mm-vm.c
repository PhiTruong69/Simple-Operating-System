// #ifdef MM_PAGING
/*
 * PAGING based Memory Management
 * Virtual memory module mm/mm-vm.c
 */
/*
Module này xử lý cấp phát và ánh xạ vùng nhớ ảo (vm_area_struct) cho tiến trình theo mô hình phân trang. Nó cung cấp các hàm:
Lấy thông tin vùng nhớ theo vmaid (get_vma_by_num)
Tính toán và khởi tạo một vùng nhớ mới từ vị trí sbrk hiện tại (get_vm_area_node_at_brk)
Kiểm tra vùng nhớ mới có bị chồng lắp không (validate_overlap_vm_area)
Tăng giới hạn vùng nhớ và ánh xạ nó vào RAM (inc_vma_limit)
*/
 #include "string.h"
 #include "mm.h"
 #include <stdlib.h>
 #include <stdio.h>
 #include <pthread.h>
 
 /*get_vma_by_num - get vm area by numID
  *@mm: memory region
  *@vmaid: ID vm area to alloc memory region
  Mục tiêu: Lấy con trỏ đến vm_area_struct tương ứng vmaid.
  Cơ chế: Duyệt danh sách liên kết mm->mmap đến khi gặp vm_id == vmaid.
  *
  */
 struct vm_area_struct *get_vma_by_num(struct mm_struct *mm, int vmaid)
 {
   struct vm_area_struct *pvma = mm->mmap;
 
   if (mm->mmap == NULL)
     return NULL;
 
   int vmait = pvma->vm_id;
 
   while (vmait < vmaid)
   {
     if (pvma == NULL)
       return NULL;
 
     pvma = pvma->vm_next;
     vmait = pvma->vm_id;
   }
 
   return pvma;
 }
 
 int __mm_swap_page(struct pcb_t *caller, int vicfpn , int swpfpn)
 {
     __swap_cp_page(caller->mram, vicfpn, caller->active_mswp, swpfpn);
     return 0;
 }
 
 /*get_vm_area_node - get vm area for a number of pages
  *@caller: caller
  *@vmaid: ID vm area to alloc memory region
  *@incpgnum: number of page
  *@vmastart: vma end
  *@vmaend: vma end
  Mục tiêu: Tính toán một vùng mới từ sbrk hiện tại.
Thao tác:
Tạo một vm_rg_struct mới bắt đầu từ cur_vma->sbrk.
Kết thúc tại sbrk + alignedsz (số byte đã căn lề).
Ý nghĩa: Đây là vùng "ứng viên" để ánh xạ RAM khi gọi inc_vma_limit.
  *
  */
 struct vm_rg_struct *get_vm_area_node_at_brk(struct pcb_t *caller, int vmaid, int size, int alignedsz)
 {
   struct vm_rg_struct * newrg;
   /* TODO retrive current vma to obtain newrg, current comment out due to compiler redundant warning*/
   struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);
   //sử dụng vm_area_struct để lấy VMA của tiến trình theo vmaid
   newrg = malloc(sizeof(struct vm_rg_struct));
 
   // TODO: update the newrg boundary
   newrg->rg_start =  cur_vma->sbrk;
   newrg->rg_end = cur_vma->sbrk + alignedsz;
   //xác định khoảng mới dựa trên breakpoint hiện tại 
 
   return newrg;
 }
 
 /*validate_overlap_vm_area  Kiểm tra nếu vùng mới bắt đầu tại vị trí khác với sbrk, nghĩa là có overlap.
  *@caller: caller
  *@vmaid: ID vm area to alloc memory region
  *@vmastart: vma end
  *@vmaend: vma end
  *
  */
 int validate_overlap_vm_area(struct pcb_t *caller, int vmaid, int vmastart, int vmaend)
 {
   //Lấy vùng bộ nhớ ảo từ vmaid trong mm của caller
   struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);
   //struct vm_area_struct *vma = caller->mm->mmap;
 
   /* TODO validate the planned memory area is not overlapped 
   - khu vực được cấp có biên [vmastart;vmaend] được đặt dựa trên sbrk hiện tại
   - nếu trùng -> vùng hoạch định không bắt đầu tại sbrk hiện tại -> có sự overlap
   */
   if(vmastart != cur_vma->sbrk) return -1;
 
   return 0;
 }
 
 /*inc_vma_limit - increase vm area limits to reserve space for new variable  Gọi get_vm_area_node_at_brk(...) để lấy vùng ứng viên.
  *@caller: caller
  *@vmaid: ID vm area to alloc memory region
  *@inc_sz: increment size
  *
  */
 int inc_vma_limit(struct pcb_t *caller, int vmaid, int inc_sz)
 {
  struct vm_rg_struct *newrg = malloc(sizeof(struct vm_rg_struct));
  int inc_amt = PAGING_PAGE_ALIGNSZ(inc_sz);
  int incnumpage = inc_amt / PAGING_PAGESZ;
  struct vm_rg_struct *area = get_vm_area_node_at_brk(caller, vmaid, inc_sz, inc_amt);
  struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);

  if (area == NULL || validate_overlap_vm_area(caller, vmaid, area->rg_start, area->rg_end) < 0)
      return -1; // Không có vùng hợp lệ hoặc bị trùng

  int old_end = cur_vma->vm_end;
  cur_vma->vm_end = area->rg_end;

  if (vm_map_ram(caller, area->rg_start, area->rg_end,
                 old_end, incnumpage, newrg) < 0)
      return -1;

  return 0; // Thành công
 }
 
 // #endif
 /*
 user_request_alloc():
    → inc_vma_limit()
        → get_vm_area_node_at_brk()
        → validate_overlap_vm_area()
        → vm_map_ram()
            → cập nhật page table + gọi MEMPHY_get_freefp()
 */