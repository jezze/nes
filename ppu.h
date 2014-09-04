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

extern void ppu_memwrite(unsigned int address,unsigned char data);
extern void ppu_checkspritehit(int scanline);
extern void ppu_renderbackground(int scanline);
extern void ppu_rendersprites();
extern unsigned int ppu_control1;
extern unsigned int ppu_control2;
extern unsigned int ppu_addr;
extern unsigned int ppu_addr_h;
extern unsigned int ppu_addr_tmp;
extern unsigned int ppu_status;
extern unsigned int ppu_status_tmp;
extern unsigned int ppu_bgscr_f;
extern unsigned int loopyT;
extern unsigned int loopyV;
extern unsigned int loopyX;
