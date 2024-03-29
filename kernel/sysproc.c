#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void) 
{
  int bitmask;

  if (argint(0, &bitmask) < 0) {
    printf("Error: failed to fetch system call argument.");
    return -1;
  }

  myproc()->tracemask = bitmask;

  return 0;
}

uint64
sys_sysinfo(void) 
{
  struct sysinfo systeminformation;
  uint64 useraddress;  // Address in user space to copy the sysinfo data to

  if (argaddr(0, &useraddress) < 0) {
    return -1;
  }

  systeminformation.freemem = kgetfreememory();  // physical memory
  systeminformation.nproc = getnumprocesses();   // # of processes that are not in the UNUSED state i.e. [RUNNING, RUNNABLE, SLEEPING]

  // copy the populated systeminformation struct to user space
  if (copyout(myproc()->pagetable, useraddress, (char *)&systeminformation, sizeof(systeminformation)) < 0) {
    return -1;
  }

  return 0;
}
