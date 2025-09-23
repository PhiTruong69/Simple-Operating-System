/*
 * Copyright (C) 2025 pdnguyen of HCMC University of Technology VNU-HCM
 */

/* Sierra release
 * Source Code License Grant: The authors hereby grant to Licensee
 * personal permission to use and modify the Licensed Source Code
 * for the sole purpose of studying while attending the course CO2018.
 */

// #ifdef MM_PAGING
/*
 * System Library
 * Memory Module Library libmem.c
 */

 #include "string.h"
 #include "mm.h"
 #include "syscall.h"
 #include "libmem.h"
 #include <stdlib.h>
 #include <stdio.h>
 #include <pthread.h>
 
static pthread_mutex_t mmvm_lock = PTHREAD_MUTEX_INITIALIZER;
 
 /*enlist_vm_freerg_list - add new rg to freerg_list
  *@mm: memory region
  *@rg_elmt: new region
  *
  */
 //Thêm một vùng nhớ (vm_rg_struct *rg_elmt) vào danh sách các vùng trống (freerg_list) của tiến trình.
 int enlist_vm_freerg_list(struct mm_struct *mm, struct vm_rg_struct *rg_elmt)
 {
   struct vm_rg_struct *rg_node = mm->mmap->vm_freerg_list;
   if (rg_elmt->rg_start >= rg_elmt->rg_end)
     return -1;  // Kiểm tra nếu vùng nhớ không hợp lệ
 
   if (rg_node != NULL)
     rg_elmt->rg_next = rg_node;
   mm->mmap->vm_freerg_list = rg_elmt;
 
   return 0;
 }
 
 /*get_symrg_byid - get mem region by region ID ; Lấy thông tin về một vùng nhớ từ bảng vùng nhớ thông qua ID vùng nhớ
  (ID này đóng vai trò là chỉ mục cho một biến trong bảng).
  *@mm: memory region
  *@rgid: region ID act as symbol index of variable
  *
  */
 struct vm_rg_struct *get_symrg_byid(struct mm_struct *mm, int rgid)
 {//Lấy vùng nhớ theo chỉ mục (biến)
   if (rgid < 0 || rgid > PAGING_MAX_SYMTBL_SZ) //30
     return NULL;
  
   return &mm->symrgtbl[rgid];
 }
 
 /*__alloc - allocate a region memory
  cấp phát một vùng nhớ (region) trong một vùng nhớ ảo (vm_area_struct).
  *@caller: caller : Trỏ đến tiến trình đang yêu cầu cấp phát bộ nhớ.
  *@vmaid: ID vm area to alloc memory region : ID của vùng nhớ ảo (Virtual Memory Area) mà ta sẽ cấp phát.
  *@rgid: memory region ID (used to identify variable in symbole table) : ID của vùng nhớ (region) mà ta sẽ ghi nhận vào bảng symrgtbl của tiến trình.
  *@size: allocated size :  Kích thước vùng nhớ cần cấp phát.
  *@alloc_addr: address of allocated memory region : Con trỏ để lưu địa chỉ bắt đầu của vùng nhớ vừa được cấp phát.
  *
  */
 int __alloc(struct pcb_t *caller, int vmaid, int rgid, int size, int *alloc_addr)
 {
  pthread_mutex_lock(&mmvm_lock);
   struct vm_rg_struct rgnode;
   if (get_free_vmrg_area(caller, vmaid, size, &rgnode) == 0) //Hàm get_free_vmrg_area cố gắng tìm vùng trống (free range) trong VMA đủ lớn.
   {
      
     caller->mm->symrgtbl[rgid].rg_start = rgnode.rg_start;
     caller->mm->symrgtbl[rgid].rg_end = rgnode.rg_end;
     *alloc_addr = rgnode.rg_start;
     pthread_mutex_unlock(&mmvm_lock);
     return 0;
   }
   struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid); //Lấy vùng heap (vmaid) hiện tại.
   int inc_sz = PAGING_PAGE_ALIGNSZ(size);
   int old_sbrk ;
   old_sbrk = cur_vma->sbrk;
   printf("old_sbrk: %d\n", old_sbrk);
   /* TODO INCREASE THE LIMIT
    * inc_vma_limit(caller, vmaid, inc_sz)
    */
   
   if(!inc_vma_limit(caller, vmaid, inc_sz)){ // Gọi inc_vma_limit để tăng sbrk
     
     // update vm_freerg_list
     if(inc_sz > size){
       struct vm_rg_struct *rgnode = malloc(sizeof(struct vm_rg_struct)); //Nếu inc_sz > size thật sự cần. phần dư này được đưa vào danh sách vùng trống freerg_list để dùng lại sau.
       rgnode->rg_start = size + old_sbrk + 1; 
       rgnode->rg_end = inc_sz + old_sbrk;
       enlist_vm_freerg_list(caller->mm, rgnode);
     }
     cur_vma->sbrk += inc_sz;
     // printf("########## sbrk: %ld\n", cur_vma->sbrk);
     /*Successful increase limit */
     caller->mm->symrgtbl[rgid].rg_start = old_sbrk;
     caller->mm->symrgtbl[rgid].rg_end = old_sbrk + size;
     *alloc_addr = old_sbrk;
     printf("alloc_addr: %d\n", *alloc_addr);
     printf("PID=%d - Region=%d - Address=%08x - Size=%d byte\n", caller->pid, rgid, *alloc_addr, size);
 
   }
   pthread_mutex_unlock(&mmvm_lock);
   return 0;
 }
 
 /*__free - remove a region memory
  *@caller: caller : Tiến trình yêu cầu giải phóng vùng nhớ.
  *@vmaid: ID vm area to alloc memory region :  ID của vùng nhớ ảo chứa vùng nhớ cần giải phóng.
  *@rgid: memory region ID (used to identify variable in symbole table) : rgid: ID của vùng bộ nhớ cần giải phóng trong bảng symrgtbl.
  *@size: allocated size
  *
  */
 
 int __free(struct pcb_t *caller, int vmaid, int rgid) //nếu rgid vượt quá giới hạn của bảng symbol, trả về lỗi.
 {

  if(rgid < 0 || rgid > PAGING_MAX_SYMTBL_SZ)
  return -1;



/* TODO: Manage the collect freed region to freerg_list */

// rgnode = ;
/*enlist the obsoleted memory region */

enlist_vm_freerg_list(caller->mm, &caller->mm->symrgtbl[rgid]);
caller->mm->symrgtbl[rgid].rg_start = 0;
caller->mm->symrgtbl[rgid].rg_end = 0;
 printf("Free region ID: %d\n", rgid);
 
return 0;
 }
 
 /*liballoc - PAGING-based allocate a region memory
  *@proc:  Process executing the instruction
  *@size: allocated size
  *@reg_index: memory region ID (used to identify variable in symbole table)
  */
 int liballoc(struct pcb_t *proc, uint32_t size, uint32_t reg_index)
 {
   if (proc == NULL || size == 0 || reg_index >= PAGING_MAX_SYMTBL_SZ) {
     return -1; // Tham số không hợp lệ
   }
 
   int addr;
   #ifdef IODUMP
   printf("===== PHYSICAL MEMORY AFTER ALLOCATION =====\n");
   print_pgtbl(proc, 0, -1); // dump toàn bộ page table
   printf("================================================================\n");
 #endif
 
   return __alloc(proc, 0, reg_index, size, &addr);
 }
 
 /*libfree - PAGING-based free a region memory
  *@proc: Process executing the instruction
  *@size: allocated size
  *@reg_index: memory region ID (used to identify variable in symbole table)
  */
 
 int libfree(struct pcb_t *proc, uint32_t reg_index)
 {
   /* TODO Implement free region */
   if (proc == NULL || reg_index >= PAGING_MAX_SYMTBL_SZ) {
     return -1; // Tham số không hợp lệ
   }
   #ifdef IODUMP
   printf("===== PHYSICAL MEMORY AFTER DEALLOCATION =====\n");
   print_pgtbl(proc, 0, -1);
   printf("================================================================\n");
 #endif
   return __free(proc, 0, reg_index);
   /* By default using vmaid = 0 */
 }
 
 /*pg_getpage - get the page in ram: Hàm pg_getpage thực hiện 
 việc lấy trang bộ nhớ (page) trong hệ thống phân trang, đồng
  thời xử lý trường hợp trang chưa có trong bộ nhớ chính (MEMRAM),
   dẫn đến việc hoán đổi (swap) giữa bộ nhớ chính và bộ nhớ phụ (MEMSWP).
  *@mm: memory region  
  *@pagenum: PGN virtual page number cần truy cập.
  *@framenum: return FPN   con trỏ ghi lại physical frame number trả về.
  *@caller: caller  tiến trình đang gọi.
  *
  */
 /*
Kiểm tra trang ảo (pgn) có đang được ánh xạ vào RAM không (dựa trên pte).
Nếu chưa có trong RAM, thực hiện swap-in trang đó từ MEMSWAP vào MEMRAM, và nếu hết RAM, chọn một victim page để swap-out.
Cập nhật page table với frame mới cho trang đang được lấy.
Trả về fpn (frame number vật lý).
 */
 int pg_getpage(struct mm_struct *mm, int pgn, int *fpn, struct pcb_t *caller) //*
 {
   uint32_t pte = mm->pgd[pgn];                 //Page Table Entry, pgd là page global directory, ở đây chính là bảng trang (page table). 
                                                // pte chứa các bit trạng thái, frame number, hoặc swap index.
   if (!PAGING_PAGE_PRESENT(pte))               //Kiểm tra trang ảo (pgn) có đang được ánh xạ vào RAM không
   { /* Page is not online, make it actively living */
     int vicpgn, swpfpn, vicfpn;
     uint32_t vicpte;
     // Target frame number in swap space
     int tgtfpn = PAGING_PTE_SWP(pte);         //Hàm này lấy swap frame number (số thứ tự frame trong swap) từ PTE.
     find_victim_page(caller->mm, &vicpgn);    //tìm 1 trang đang ở RAM để đẩy ra SWAP.
     if (vicpgn == -1)
         return -1;
     vicpte = mm->pgd[vicpgn];                 // lấy thông tin của trang nạn nhân
     vicfpn = PAGING_PTE_PGN(vicpte);          // frame number mà vicpgn đang nằm trên RAM.
     /* Get free frame in MEMSWP */
     MEMPHY_get_freefp(caller->active_mswp, &swpfpn); // frame trong SWAP để chứa dữ liệu của victim. Tìm một frame trống trong SWAP
     if (swpfpn == -1)
         return -1;
     /* Do swap frame from MEMRAM to MEMSWP and vice versa */
     /* Copy victim frame to swap */
     __swap_cp_page(caller->mram, vicfpn, *(caller->mswp), swpfpn);     //Copy toàn bộ page từ RAM sang SWAP tại swpfpn.
     /* Copy target frame from swap to mem */
     __swap_cp_page(*(caller->mswp), tgtfpn, caller->mram, vicfpn);     //Dữ liệu của pgn từ tgtfpn trong SWAP sẽ được nạp vào vicfpn trong RAM.
     /* Update page table */
     pte_set_swap(&mm->pgd[vicpgn], 1, swpfpn); // victim giờ nằm ở swap
     /* Update its online status of the target page */
     pte_set_fpn(&mm->pgd[pgn], vicfpn);                                //Cho trang mới (pgn)
     enlist_pgn_node(&caller->mm->fifo_pgn, pgn);                       // Ghi lại page mới vừa vào RAM vào hàng đợi FIFO
   }
 
   *fpn = PAGING_FPN(mm->pgd[pgn]); //Trả lại frame number vật lý cho người gọi
 
   return 0;
 }
 
 /*pg_getval - read value at given offset // Đọc dữ liệu từ địa chỉ ảo
  *@mm: memory region
  *@addr: virtual address to acess
  *@value: value
  *
  */
 int pg_getval(struct mm_struct *mm, int addr, BYTE *data, struct pcb_t *caller)
 {
   int pgn = PAGING_PGN(addr);	  // lấy số trang ở địa chỉ ảo 
   int off = PAGING_OFFST(addr); // lấy opset trong trang 
   int fpn;
   //Gọi pg_getpage để lấy số frame vật lý tương ứng với trang pgn.
   //Nếu trang chưa nằm trong RAM thì sẽ swap-in từ SWAP.
   //Nếu trang không hợp lệ (ví dụ chưa cấp phát), trả về lỗi.
   if (pg_getpage(mm, pgn, &fpn, caller) != 0)
     return -1; 
 /*
 Dịch số frame vật lý thành địa chỉ vật lý thực tế bằng cách:
Dịch trái fpn để ra base address của frame.
Cộng thêm offset để ra vị trí chính xác trong frame.
 */
   int phyaddr = (fpn << PAGING_ADDR_FPN_LOBIT) + off; // dich trái với số bit bằng số kt trang.
   MEMPHY_read(caller->mram, phyaddr, data);
   return 0;
 }
 
 /*pg_setval - write value to given offset
  *@mm: memory region
  *@addr: virtual address to acess
  *@value: value
  *
  */
 int pg_setval(struct mm_struct *mm, int addr, BYTE value, struct pcb_t *caller)
 {
   int pgn = PAGING_PGN(addr);
   int off = PAGING_OFFST(addr);
   int fpn;
 
   /* Get the page to MEMRAM, swap from MEMSWAP if needed */
   if (pg_getpage(mm, pgn, &fpn, caller) != 0)
     return -1; /* invalid page access */
 
   int phyaddr = (fpn << PAGING_ADDR_FPN_LOBIT) + off;
 
   MEMPHY_write(caller->mram, phyaddr, value);
 
   return 0;
 }
 
 /*__read - read value in region memory // Đọc 1 byte trong biến (vùng đã cấp phát)
  *@caller: caller
  *@vmaid: ID vm area to alloc memory region
  *@offset: offset to acess in memory region
  *@rgid: memory region ID (used to identify variable in symbole table)
  *@size: allocated size
  *
  */
 int __read(struct pcb_t *caller, int vmaid, int rgid, int offset, BYTE *data)
 {
   struct vm_rg_struct *currg = get_symrg_byid(caller->mm, rgid);  //Lấy vùng địa chỉ được biểu diễn bởi rgid.
   struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);
 
   if (currg == NULL || cur_vma == NULL) /* Invalid memory identify */
     return -1;
 
   pg_getval(caller->mm, currg->rg_start + offset, data, caller);
   return 0;
 }
 
 /*libread - PAGING-based read a region memory */
 int libread(
     struct pcb_t *proc, // Tiến trình thực thi lệnh
     uint32_t source,    // Chỉ số vùng nhớ nguồn (rgid)
     uint32_t offset,    // Source address = [source] + [offset]
     uint32_t *destination) // Nơi lưu dữ liệu đọc được
 {
  BYTE data;
  int size_rg = proc->mm->symrgtbl[source].rg_end - proc->mm->symrgtbl[source].rg_start;
//Mỗi vùng nhớ (region) có địa chỉ bắt đầu – kết thúc trong bảng symrgtbl
  if (offset >= size_rg) {  
      return -1;
  }

  int val = __read(proc, 0, source, offset, &data);
  
  if (val != 0) {
      return -1;  
  }

  *destination = (uint32_t)data;
   /* TODO update result of reading action*/
 #ifdef IODUMP
   printf("read region=%d offset=%d value=%d\n", source, offset, data); //In thông tin đọc ra màn hình (vùng, offset, value).
 #ifdef PAGETBL_DUMP
   print_pgtbl(proc, 0, -1); // print max TBL
   printf("================================================================\n");
 #endif
   MEMPHY_dump(proc->mram);
 #endif
 
   return val;
 }
 
 /*__write - write a region memory
  *@caller: caller
  *@vmaid: ID vm area to alloc memory region
  *@offset: offset to acess in memory region
  *@rgid: memory region ID (used to identify variable in symbole table)
  *@size: allocated size
  *
  */
 int __write(struct pcb_t *caller, int vmaid, int rgid, int offset, BYTE value)
 {
 
   struct vm_rg_struct *currg = get_symrg_byid(caller->mm, rgid);
   struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);
 
   if (currg == NULL || cur_vma == NULL || currg->rg_start >= currg->rg_end) /* Invalid memory identify */
     return -1;
 
   pg_setval(caller->mm, currg->rg_start + offset, value, caller);
 
   return 0;
 }
 
 /*libwrite - PAGING-based write a region memory */
 int libwrite(
 
     struct pcb_t *proc,   // Process executing the instruction
     BYTE data,            // Data to be wrttien into memory
     uint32_t destination, // Index of destination register
     uint32_t offset)
 {
 #ifdef IODUMP
   printf("write region=%d offset=%d value=%d\n", destination, offset, data);
 #ifdef PAGETBL_DUMP
   print_pgtbl(proc, 0, -1); // print max TBL
   printf("================================================================\n");
 #endif
   MEMPHY_dump(proc->mram);
 #endif
 
   return __write(proc, 0, destination, offset, data);
 }
 
 /*free_pcb_memphy - collect all memphy of pcb
  *@caller: caller
  *@vmaid: ID vm area to alloc memory region
  *@incpgnum: number of page
  */
 int free_pcb_memph(struct pcb_t *caller)
 {
   int pagenum, fpn;
   uint32_t pte;
 
   for (pagenum = 0; pagenum < PAGING_MAX_PGN; pagenum++)
   {
     pte = caller->mm->pgd[pagenum];
 
     if (!PAGING_PAGE_PRESENT(pte))
     {
       fpn = PAGING_PTE_FPN(pte);
       MEMPHY_put_freefp(caller->mram, fpn);
     }
     else
     {
       fpn = PAGING_PTE_SWP(pte);
       MEMPHY_put_freefp(caller->active_mswp, fpn);
     }
   }
 
   return 0;
 }
 
 /*find_victim_page - find victim page
 Hàm này dùng để chọn một trang đang có trong RAM làm nạn nhân (victim)  
 tức là sẽ bị swap-out để nhường chỗ cho trang khác cần được load vào RAM.
  *@caller: caller
  *@pgn: return page number
  *
  */
 int find_victim_page(struct mm_struct *mm, int *retpgn)
 {
    struct pgn_t *pg = mm->fifo_pgn;
  if(!pg) return -1; // Không có page nào để chọn làm victim
  if(!pg->pg_next){    // trg hợp có 1 page 
    *retpgn = pg->pgn;
    free(pg);
    return 0;
  }
  struct pgn_t *prev_pg = mm->fifo_pgn; 
  /* TODO: Implement the theorical mechanism to find the victim page */
  while(pg->pg_next) {
    prev_pg = pg;
    pg = pg -> pg_next;
  }
  *retpgn = pg->pgn;
  prev_pg->pg_next = NULL; 
  free(pg);
  return 0;
  
 }
 
 /*get_free_vmrg_area - get a free vm region
  *@caller: caller
  *@vmaid: ID vm area to alloc memory region
  *@size: allocated size
  *
  */
 
 /*
 Hàm get_free_vmrg_area có nhiệm vụ tìm một vùng nhớ trống (free region)
 trong danh sách vùng nhớ (vm_freerg_list) của một vùng nhớ ảo (vm_area_struct).
 Nếu tìm thấy vùng nhớ phù hợp với kích thước size yêu cầu, nó sẽ cập nhật thông
 tin của vùng nhớ mới (newrg) và điều chỉnh danh sách vùng nhớ trống
 */
 int get_free_vmrg_area(struct pcb_t *caller, int vmaid, int size, struct vm_rg_struct *newrg)
 {
   struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);
   struct vm_rg_struct *rgit = cur_vma->vm_freerg_list;
 
   if (rgit == NULL)
     return -1;
 
   /* Probe unintialized newrg */
   newrg->rg_start = newrg->rg_end = -1;
 
   /* TODO Traverse on list of free vm region to find a fit space */
   // while (...)
   //  ..
   while (rgit != 0)
   {
     if (rgit->rg_start + size <= rgit->rg_end) // vùng nhớ đủ lớn, đc chọn
     {
       newrg->rg_start = rgit->rg_start;
       newrg->rg_end = rgit->rg_start + size; // Cập nhật vùng nhớ mới
       if (rgit->rg_start + size < rgit->rg_end)
       {
         rgit->rg_start = rgit->rg_start + size; // nếu còn vùng nhớ trống, cập nhật cho rg ch qua sd
       }
       else
       { /*Use up all space, remove current node */
         /*Clone next rg node */
         struct vm_rg_struct *nextrg = rgit->rg_next;
         /*Cloning */
         if (nextrg != NULL) // Nếu có vùng nhớ tiếp theo (nextrg): Sao chép thông tin từ nextrg vào rgit, sau đó xóa nextrg.
         {
           rgit->rg_start = nextrg->rg_start;
           rgit->rg_end = nextrg->rg_end;
           rgit->rg_next = nextrg->rg_next;
           free(nextrg);
         }
         else
         {                                /*End of free list */
           rgit->rg_start = rgit->rg_end; // dummy, size 0 region
           rgit->rg_next = NULL;
         }
       }
       break;
     }
     else
     {
       rgit = rgit->rg_next; // Traverse next rg
     }
   }
 
   if (newrg->rg_start == -1) // new region not found
     return -1;
   return 0;
 }
 
 // #endif
 