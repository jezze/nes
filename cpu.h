int cpu_irq(int cycles, unsigned char *memory);
int cpu_nmi(int cycles, unsigned char *memory);
void cpu_reset(unsigned char *memory);
int cpu_execute(int cycles, unsigned char *memory);
