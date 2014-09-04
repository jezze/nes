extern int cpu_irq(int cycles);
extern int cpu_nmi(int cycles);
extern void cpu_reset(void);
extern int cpu_execute(int cycles);
extern unsigned char *memory;
extern unsigned int tmp;
