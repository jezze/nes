unsigned char memory_read(unsigned int address);
void write_memory(unsigned int address,unsigned char data);
void set_input(int pad_key);
void clear_input(int pad_key);
void halt();

extern unsigned char *romcache;
extern long romlen;
extern unsigned int tmp;
extern unsigned char memory[65536];
