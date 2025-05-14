#include "graphics.h"
#include <lcom/lcf.h>

static void *video_mem;

int(set_to_video_mode)(uint16_t submode) {
  reg86_t reg86;
  // set up with 0s
  memset(&reg86, 0, sizeof(reg86));

  reg86.intno = 0x10;

  // setting to video (0x0003 for text)
  reg86.ax = 0x4F02;
  // BIT 14 is linear mem mapping
  reg86.bx = submode | BIT(14);

  if (sys_int86(&reg86) != 0) {
    printf("sys_int86 error");
    return 1;
  }
  return 0;
}

int(set_to_text_mode)() {
  reg86_t reg86;
  // set up with 0s
  memset(&reg86, 0, sizeof(reg86));

  // setting to video (0x0003 for text)
  reg86.ah = 0x00;
  reg86.al = 0x03;
  // reg86.ax = 0x0003;
  // BIT 14 is linear mem mapping
  reg86.bx = 0x0000;

  reg86.intno = 0x10;

  if (sys_int86(&reg86) != 0) {
    printf("sys_int86 error");
    return 1;
  }
  return 0;
}

int(set_frame_buffer)(uint16_t mode) {
  // set the external mode info to 0
  memset(&mode_info, 0, sizeof(mode_info));
  // Returns information on the input VBE mode, including screen dimensions, color depth and VRAM physical address
  // Gets phys pointer, x res, y res, bits/pixel, bitsize of color mask, position of color mask, memmodel and color
  if (vbe_get_mode_info(mode, &mode_info))
    return 1;

  // Calculate mem size
  unsigned int bytes_per_pixel = mode_info.BitsPerPixel / 8;

  // frame size in bytes is x * y * size of each pixel
  unsigned int frame_size = mode_info.XResolution * mode_info.YResolution * bytes_per_pixel;

  // Put the actual values to the mem range
  struct minix_mem_range mr;
  mr.mr_base = mode_info.PhysBasePtr;
  mr.mr_limit = mr.mr_base + frame_size;

  int r;
  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  // Map physical mem to virtual

  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, frame_size);

  if (video_mem == MAP_FAILED || video_mem == NULL)
    panic("couldn't map video memory");

  return 0;
}
