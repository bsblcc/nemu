#include <am.h>
#include <x86.h>
#include <klib.h>
static _Context* (*user_handler)(_Event, _Context*) = NULL;

void vectrap();
void vecnull();



/*
eflags
cs
eip for return0


errorcode           
irq
eip for return1


eax
ecx
edx
ebx
esp
ebp
esi
edi
prot
esp
eip for return2

*/
_Context* irq_handle(_Context *tf) {
  for (int i = 0; i < 50; i++) {
    printf("%d\n", tf->tmp[i]);
  }
  _Context *next = tf; 
  if (user_handler) {
    _Event ev = {0};
    switch (tf->irq) {
      default: ev.event = _EVENT_ERROR; break;
    }

    next = user_handler(ev, tf);
    if (next == NULL) {
      next = tf;
    }
  }

  return next;
}

static GateDesc idt[NR_IRQ];

int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i] = GATE(STS_TG32, KSEL(SEG_KCODE), vecnull, DPL_KERN);
  }

  // -------------------- system call --------------------------
  idt[0x81] = GATE(STS_TG32, KSEL(SEG_KCODE), vectrap, DPL_KERN);

  set_idt(idt, sizeof(idt));

  // register event handler
  user_handler = handler;

  return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
  return NULL;
}

void _yield() {
  asm volatile("int $0x81");
}

int _intr_read() {
  return 0;
}

void _intr_write(int enable) {
}
