/*
 * Copyright (C) 2025 pdnguyen of HCMC University of Technology VNU-HCM
 */

/* Sierra release
 * Source Code License Grant: The authors hereby grant to Licensee
 * personal permission to use and modify the Licensed Source Code
 * for the sole purpose of studying while attending the course CO2018.
 */

 #include "common.h"
 #include "syscall.h"
 #include "stdio.h"
 #include "libmem.h"
 
 #include<string.h>  
 #include<stdlib.h>
 #include "queue.h"
 
 int __sys_killall(struct pcb_t *caller, struct sc_regs* regs)
 {
     char proc_name[100];
     uint32_t data;
 
     //hardcode for demo only
     uint32_t memrg = regs->a1;
     
     /* TODO: Get name of the target proc */
     //proc_name = libread..
     /* Đọc chuỗi tên tiến trình (kiểu char*) được lưu trong một vùng nhớ ảo của tiến trình gọi syscall,
      với chỉ số vùng nhớ được truyền qua thanh ghi reg -> a1.
     */
    int i = 0;
    data = 0;
    while(data != -1){
        libread(caller, memrg, i, &data);
        proc_name[i]= (char)data;
        if(data == -1) proc_name[i]='\0';
        i++;
    }
    printf("The procname retrieved from memregionid %d is \"%s\"\n", memrg, proc_name);
 
     /* TODO: Traverse proclist to terminate the proc
      *       stcmp to check the process match proc_name
      */
         
     // Duyệt running_list 
     if (caller->running_list && caller->running_list->size > 0) {
         struct pcb_t **procs = caller->running_list->proc;
         int new_size = 0;
         for (int i = 0; i < caller->running_list->size; i++) {
             if (!procs[i]) continue;
             if (strcmp(procs[i]->path, proc_name) != 0) {
                 procs[new_size++] = procs[i]; // Giữ lại
             } else {
                 printf("Terminating running process %d (%s)\n", 
                     procs[i]->pid, procs[i]->path);
                 free(procs[i]->mm);
                 free(procs[i]);
                 procs[i] = NULL;
             } 
         caller->running_list->size = new_size;
         }
     }
 
     // Duyệt ready_queue (theo từng mức ưu tiên)
     if(caller->ready_queue && caller->ready_queue->size > 0) {
         struct queue_t *q = caller->ready_queue;
         for (int i = 0; i < MAX_PRIO; i++) {
             struct pcb_t **procs = q[i].proc;
             int new_size = 0;
             for (int j = 0; j < q[i].size; j++) {
                 if (!procs[j]) continue;
                 if (strcmp(procs[j]->path, proc_name) != 0) {
                     procs[new_size++] = procs[j]; // Giữ lại
                 } else {
                     printf("Terminating ready process %d (%s)\n", 
                         procs[j]->pid, procs[j]->path);
                     free(procs[j]->mm);
                     free(procs[j]);
                     procs[j] = NULL;
                 }
             }
             q[i].size = new_size;
         }
     }
 
     /* TODO Maching and terminating 
      *       all processes with given
      *        name in var proc_name
      */
     
     return 0; 
 }
 