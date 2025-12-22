#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

#define PAGES 16

void print_pgtbl();
void pgaccess_test();

int main(int argc, char *argv[])
{
  print_pgtbl();
  pgaccess_test();
  printf("pgtbltest: OK\n");
  exit(0);
}

/* -------------------------------------------------- */

void print_pte(uint64 va)
{
  pte_t pte = (pte_t)pgpte((void *)va);
  printf("va 0x%lx pte 0x%lx pa 0x%lx perm 0x%lx\n",
         va, pte, PTE2PA(pte), PTE_FLAGS(pte));
}

void print_pgtbl()
{
  printf("print_pgtbl starting\n");

  for (uint64 i = 0; i < 10; i++)
    print_pte(i * PGSIZE);

  uint64 top = MAXVA / PGSIZE;
  for (uint64 i = top - 10; i < top; i++)
    print_pte(i * PGSIZE);

  printf("print_pgtbl: OK\n");
}

/* -------------------------------------------------- */

void pgaccess_test()
{
  printf("pgaccess_test starting\n");

  char *buf = sbrk(PAGES * PGSIZE);
  if (buf == (char *)-1)
  {
    printf("sbrk failed\n");
    exit(1);
  }

  // touch some pages
  buf[0] = 1;
  buf[2 * PGSIZE] = 1;
  buf[5 * PGSIZE] = 1;

  uint64 mask = 0;
  if (pgaccess(buf, PAGES, &mask) < 0)
  {
    printf("pgaccess failed\n");
    exit(1);
  }

  printf("pgaccess mask = 0x%lx\n", mask);

  // expect bits 0,2,5 set
  if ((mask & (1L << 0)) == 0 ||
      (mask & (1L << 2)) == 0 ||
      (mask & (1L << 5)) == 0)
  {
    printf("pgaccess wrong mask\n");
    exit(1);
  }

  printf("pgaccess_test: OK\n");
}
