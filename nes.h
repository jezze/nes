unsigned char ram_read(unsigned int address);
void ram_write(unsigned int address,unsigned char data);
void input_set(int key);
void input_clear(int key);
void halt();

extern long romlen;
