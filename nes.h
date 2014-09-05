unsigned char ram_read(unsigned int address);
void ram_write(unsigned int address,unsigned char data);
void input_set(int key);
void input_clear(int key);
void halt();

#define increment_32		            (ppu_control1 & 0x04)
#define sprite_addr_hi                  (ppu_control1 & 0x08)
#define background_addr_hi	            (ppu_control1 & 0x10)
#define sprite_16                       (ppu_control1 & 0x20)
#define exec_nmi_on_vblank	            (ppu_control1 & 0x80)
#define monochrome_on		            (ppu_control2 & 0x01)
#define background_clipping_off	        (ppu_control2 & 0x02)
#define sprite_clipping_off	            (ppu_control2 & 0x04)
#define background_on                   (ppu_control2 & 0x08)
#define sprite_on                       (ppu_control2 & 0x10)
#define vram_write_flag		            (ppu_status & 0x10)
#define scanline_sprite_count	        (ppu_status & 0x20)
#define sprite_zero		                (ppu_status & 0x40)
#define vblank_on                       (ppu_status & 0x80)

extern long romlen;
