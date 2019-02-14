#include <lcom/lcf.h>

#include "videocard.h"

/* GLOBAL VARIABLES*/

static uint16_t hres;              /* XResolution */
static uint16_t vres;              /* YResolution */
static char *video_mem;            /* frame-buffer VM address*/
static unsigned int vram_base;     /* VRAM's physical address*/
static unsigned int vram_size;     /* VRAM's size, in bytes */
static unsigned int bitsPerPixel;  /* number of bits per pixel*/
static unsigned int bytesPerPixel; /* number of bytes per pixel*/
static xpm_image_t background;     /* background of the presented frame */

static char *scene;                /* frame building buffer */

static char *aux_bg_buffer;        /* auxiliar buffer for backgrounds with immutable sprites */

// number of bits of the masks of the components of each pixel
static uint8_t RedMaskSize, GreenMaskSize, BlueMaskSize, RsvdMaskSize;
// bit correspoding to the start of the component of a pixel
static uint8_t RedFieldPosition, GreenFieldPosition, BlueFieldPosition;

int scene_to_aux_buffer() {


  memcpy(aux_bg_buffer, scene, vram_size);



  return 0;
}

int draw_aux_buffer() {

  memcpy((char *) scene, (char *) aux_bg_buffer, vram_size);
  
  return 0;
} 



/* FUNCTIONS */

int init_vbe_mode(uint16_t mode) {

  struct reg86u register_struct;

  memset(&register_struct, 0, sizeof(register_struct)); // reset the struct

  register_struct.u.b.ah = VBE_ID;             // VBE call
  register_struct.u.b.al = VBE_SET_MODE;       // function 02 -- set VBE mode
  register_struct.u.w.bx = LIN_FR_BUFF | mode; // set bit 14: linear framebuffer
  register_struct.u.b.intno = BIOS_VC_SER;     // set BIOS service to video card

  // execute BIOS call
  if (sys_int86(&register_struct) != OK)
    return -1;

  // return the result of the BIOS call, 0 if sucessfull, 1,2 or 3 if unsucessfull
  return register_struct.u.b.ah;
}

int vbe_mode_info_get(uint16_t mode, vbe_mode_info_t *vmi_p) {

  struct reg86u register_struct;
  mmap_t lowMem_map = {0, 0, 0};

  // map the low memory area in the process' physical memory address
  void *baseAdd = lm_init(false);

  if (baseAdd == NULL)
    return -1;

  // allocate memory in the lower MB for the mode info block
  if (lm_alloc(sizeof(vbe_mode_info_t), &lowMem_map) == NULL)
    return -2;

  // get the physical and virtual memory addresses of the mode info block
  phys_bytes physAddress = lowMem_map.phys;

  memset(&register_struct, 0, sizeof(register_struct)); // reset the struct

  // VBE BIOS call "preparation"

  register_struct.u.b.ah = VBE_ID;               // VBE call
  register_struct.u.b.al = VBE_GET_MODE_INFO;    // function 01 -- get VBE mode
  register_struct.u.w.cx = mode;                 // select mode
  register_struct.u.b.intno = BIOS_VC_SER;       // set BIOS service to video card
  register_struct.u.w.es = PB2BASE(physAddress); // base segment value of physical address
  register_struct.u.w.di = PB2OFF(physAddress);  // offset value of physical address

  if (sys_int86(&register_struct) != OK) // run BIOS call
    return -3;

  // check the result of the BIOS call
  int biosCallResult = register_struct.u.b.ah;

  if (biosCallResult != OK)
    return biosCallResult;

  // initialize mode info struct
  *vmi_p = *(vbe_mode_info_t *) lowMem_map.virt;

  // free alocated memory
  if (!lm_free(&lowMem_map))
    return -4;

  return 0;
}

void *(vg_init)(uint16_t mode) {

  // struct holding the info about the VBE mode
  vbe_mode_info_t vbe_mi;

  // initialize the mode info struct
  vbe_mode_info_get(mode, &vbe_mi);

  // initialze global variables

  hres = vbe_mi.XResolution;
  vres = vbe_mi.YResolution;

  if (mode == INDEXED_COLOR_MODE)
    bitsPerPixel = 8;
  else
    bitsPerPixel = vbe_mi.RedMaskSize + vbe_mi.GreenMaskSize + vbe_mi.BlueMaskSize + vbe_mi.RsvdMaskSize;

  bytesPerPixel = bitsPerPixel / BITS_PER_BYTE;
  vram_base = (unsigned int) vbe_mi.PhysBasePtr;
  vram_size = hres * vres * bytesPerPixel;

  RedMaskSize = vbe_mi.RedMaskSize;
  GreenMaskSize = vbe_mi.GreenMaskSize;
  BlueMaskSize = vbe_mi.BlueMaskSize;
  RsvdMaskSize = vbe_mi.RsvdMaskSize;

  RedFieldPosition = vbe_mi.RedFieldPosition;
  GreenFieldPosition = vbe_mi.GreenFieldPosition;
  BlueFieldPosition = vbe_mi.BlueFieldPosition;

  struct minix_mem_range mr;

  // initialize memory range struct
  mr.mr_base = vbe_mi.PhysBasePtr;
  mr.mr_limit = mr.mr_base + vram_size;

  // get permission to map the memory
  int permissionGranted = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr);

  // if permission failed, terminate
  if (permissionGranted != OK)
    return NULL;

  // map the memory for the video memory and store it in the global variable
  video_mem = (char *) vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

  // if the mapping failed, terminate
  if (video_mem == MAP_FAILED) {

    video_mem = NULL;
    return NULL;
  }

  // set graphics mode
  int validInit = init_vbe_mode(mode);

  // if the mode initialization failed, terminate
  if (validInit != OK)
    return NULL;

  // ATENÇÂO AQUI

  scene = (char *) malloc(bytesPerPixel * hres * vres);
  aux_bg_buffer = (char *) malloc(bytesPerPixel * hres * vres);

  return video_mem;
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {

  // check if the current state of th video mem is valid
  if (video_mem == NULL)
    return 1;

  /*// check if line is in bounds
  if(x + len >= hres)
    return 2; */

  // set currentPtr to point the the first pixel of the line
  char *currentPtr = scene + (y * hres + x) * bytesPerPixel;

  // draw each pixel of the line
  for (uint16_t i = 0; i < len; i++) {

    if (x + i == hres)
      break;
    // individually write each byte of the pixel color
    for (unsigned int j = 0; j < bytesPerPixel; j++) {
      *currentPtr = (uint8_t)(color >> j * 8);
      currentPtr++;
    }
  }

  return 0;
}

int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {

  /* // check height of the rectangle is in bounds
   if(y + height >= vres)
    return 1; */

  // draw the lines that compose the rectangle
  for (uint16_t i = 0; i < height; i++) {
    if (y + i == vres)
      break;
    int validDraw = vg_draw_hline(x, y + i, width, color);

    // if the line wasn't sucessfully drawn, terminate
    if (validDraw != OK)
      return validDraw;
  }

  return 0;
}

uint16_t getHRes() { return hres; }
uint16_t getVRes() { return vres; }

unsigned int getBitsPP() { return bitsPerPixel; }
unsigned int getBytesPP() { return bytesPerPixel; }

void getColorValues(uint32_t first, uint32_t *fRed, uint32_t *fGreen, uint32_t *fBlue) {

  // get the blue component of 'first'
  *fBlue = (first >> BlueFieldPosition) & (makeMask(BlueMaskSize, true));

  // get the green component of 'first'
  *fGreen = (first >> GreenFieldPosition) & (makeMask(GreenMaskSize, true));

  // get the red component of 'first'
  *fRed = (first >> RedFieldPosition) & (makeMask(RedMaskSize, true));
}

uint32_t getColor(uint16_t mode, uint8_t no_rectangles, uint8_t row, uint8_t column, uint32_t first, uint8_t step) {

  uint32_t resultColor = 0;
  uint32_t red, blue, green = 0;
  uint32_t fRed, fGreen, fBlue = 0;

  // split the 'first' color into it's components
  getColorValues(first, &fRed, &fGreen, &fBlue);

  // calculate the resulting color depending on the color mode
  if (mode == INDEXED_COLOR_MODE) {

    resultColor = (first + (row * no_rectangles + column) * step) % (1 << bitsPerPixel);
  }
  else {

    red = (fRed + column * step) % (1 << RedMaskSize);
    resultColor |= red;
    resultColor = resultColor << GreenMaskSize;

    green = (fGreen + row * step) % (1 << GreenMaskSize);
    resultColor |= green;
    resultColor = resultColor << BlueMaskSize;

    blue = (fBlue + (column + row) * step) % (1 << BlueMaskSize);
    resultColor |= blue;
  }

  return resultColor;
}

int draw_xpm_legacy(const char *xpm[], uint16_t xi, uint16_t yi) {

  int width, height;

  // convert the xmp into a pixmap
  char *pixmap = read_xpm(xpm, &width, &height);

  // check if the conversion was sucessfull
  if (pixmap == NULL)
    return 1;

  // check if the height of the pixmap is in bounds
  if (yi >= vres)
    return 0;

  // check if the width of the pixmap is in bounds
  if (xi >= hres)
    return 0;

  // store in currPtr the mem position corresponding to the top left corner of the picture
  char *currPtr = video_mem + (yi * hres + xi) * bytesPerPixel;

  // draw the pixmap
  for (uint16_t i = 0; i < height; i++) {

    if (yi + i == vres)
      break;
    currPtr = video_mem + ((yi + i) * hres + xi) * bytesPerPixel;

    for (uint16_t j = 0; j < width; j++) {

      if (xi + j >= hres) {
        pixmap++;
      }
      else {

        *currPtr = *pixmap;

        currPtr++;
        pixmap++;
      }
    }
  }

  return 0;
}

bool xpm_check_out_of_bounds(xpm_image_t *xpm, int16_t x, int16_t y) {

  if (x >= hres) {
    return true;
  }

  if (y >= vres) {
    return true;
  }

  if ((x + xpm->width) < 0) {
    return true;
  }

  if ((y + xpm->height) < 0) {
    return true;
  }

  return false;
}

int draw_xpm(xpm_image_t *xpm, int16_t xi, int16_t yi) {

  int width = xpm->width, height = xpm->height;
  
  // convert the xmp into a pixmap
  char *pixmap = (char *) xpm->bytes;

  // check if the conversion was sucessfull
  if (pixmap == NULL)
    return 1;

  // store in currPtr the mem position corresponding to the top left corner of the picture

  char *currPtr;

  //acount for out of the screen xpm
  if (yi >= vres)
    return 0;

  // acount for partial drawing out of the top bound
  if (yi < 0) {

    pixmap += width * abs(yi)*bytesPerPixel;

    // acount for partial drawing out of the left bound
    if (xi < 0)
      currPtr = (char *) scene;
    else
      currPtr = (char *) scene + xi*bytesPerPixel;

    // draw the pixmap
    for (uint16_t i = 0; i < (height - abs(yi)); i++) {

      // account for partial drawing out of the bottom bound
      if (yi + i == vres)
        break;

      // acount for partial drawing out of the left bound
      if (xi < 0)
        currPtr = (char *) scene + i * hres * bytesPerPixel;
      else
        currPtr = (char *) scene + (i * hres + xi)*bytesPerPixel;

      int temp = 0;
      for (uint16_t j = 0; j < width; j++) {

        // acount for partial drawing out of the left bound
        if (xi < 0) {
          if (temp < abs(xi)) {
            temp++;
            pixmap+=bytesPerPixel;
            continue;
          }
        }

        // acount for partial drawing out of the right bound
        if (xi + j >= hres) {
          pixmap+=bytesPerPixel;
        }
        else {
          
          int* currentPXMP = (int*)pixmap;
          // check for transparent pixels
          if (!(*currentPXMP & BIT(15))){

            for(unsigned int i = 0; i < bytesPerPixel;i++){

              *currPtr = *pixmap;
              currPtr++;
              pixmap++;
            }

          }
          else{
            currPtr+= bytesPerPixel;
            pixmap+=bytesPerPixel;
          } 
          
        }
      }
    }
  }
  else {
      // acount for partial drawing out of the left bound
    if (xi < 0)
      currPtr = (char *) scene + (yi * hres * bytesPerPixel);
    else
      currPtr = (char *) scene + (yi * hres + xi) * bytesPerPixel;

    // draw the pixmap
    for (uint16_t i = 0; i < height; i++) {

      if (yi + i == vres)
        break;

      // acount for partial drawing out of the left bound
      if (xi < 0)
        currPtr = (char *) scene + ((yi + i) * hres)*bytesPerPixel;
      else
        currPtr = (char *) scene + ((yi + i) * hres + xi) * bytesPerPixel;

      int temp = 0;
      for (uint16_t j = 0; j < width; j++) {

        // acount for partial drawing out of the left bound
        if (xi < 0) {
          if (temp < abs(xi)) {
            temp++;
            pixmap+=bytesPerPixel;
            continue;
          }
        }

        // acount for partial drawing out of the right bound
        if (xi + j >= hres) {
          pixmap+=bytesPerPixel;
        }
        else {
          // check for transparent pixels
          int* currentPXMP = (int*)pixmap;
          if (!(*currentPXMP & BIT(15))){

            for(unsigned int i = 0; i < bytesPerPixel;i++){

              *currPtr = *pixmap;
              currPtr++;
              pixmap++;
            }
          }
          else{

            currPtr+=bytesPerPixel;
            pixmap+=bytesPerPixel;
          }  

          
        }
      }
    }
  }

  return 0;
}

char *getVideo_mem() {

  return video_mem;
}

int vbe_controller_info_get(vg_vbe_contr_info_t *vci_p) {

  struct reg86u register_struct;
  mmap_t lowMem_map = {0, 0, 0};

  // map the low memory area in the process' physical memory address
  void *baseAdd = lm_init(false);

  if (baseAdd == NULL)
    return -1;

  // allocate memory in the lower MB for the mode info block
  if (lm_alloc(sizeof(Mode_Info_Block_t), &lowMem_map) == NULL)
    return -2;

  // get the physical and virtual memory addresses of the mode info block
  phys_bytes physAddress = lowMem_map.phys;

  Mode_Info_Block_t *tempVirt = (Mode_Info_Block_t *) lowMem_map.virt;

  // Initializa VBE signature as 'VBE2'

  tempVirt->VBESignature[0] = 'V';
  tempVirt->VBESignature[1] = 'B';
  tempVirt->VBESignature[2] = 'E';
  tempVirt->VBESignature[3] = '2';

  memset(&register_struct, 0, sizeof(register_struct)); // reset the struct

  // VBE BIOS call "preparation"

  register_struct.u.b.ah = VBE_ID;               // VBE call
  register_struct.u.b.al = VBE_GET_CTLR_INFO;    // function 00 -- get VBE controller info
  register_struct.u.b.intno = BIOS_VC_SER;       // set BIOS service to video card
  register_struct.u.w.es = PB2BASE(physAddress); // MSW of pointer to Mode info block
  register_struct.u.w.di = PB2OFF(physAddress);  // LSW of pointer to Mode info block

  if (sys_int86(&register_struct) != OK) // run BIOS call
    return -3;

  if (register_struct.u.b.ah != OK)
    return -4;

  // VBE Controller Info struct initialization;

  vci_p->TotalMemory = (uint32_t) tempVirt->TotalMemory * 64;

  for (int i = 0; i < 4; i++) {
    vci_p->VBESignature[i] = tempVirt->VBESignature[i];
  }

  for (int i = 0; i < 2; i++) {
    vci_p->VBEVersion[i] = tempVirt->VBEVersion[i];
  }

  vci_p->OEMString = (char *) FarPtr_to_Ptr(baseAdd, tempVirt->OEMString);

  vci_p->OEMProductNamePtr = (char *) FarPtr_to_Ptr(baseAdd, tempVirt->OEMProductNamePtr);

  vci_p->OEMProductRevPtr = (char *) FarPtr_to_Ptr(baseAdd, tempVirt->OEMProductRevPtr);

  vci_p->OEMVendorNamePtr = (char *) FarPtr_to_Ptr(baseAdd, tempVirt->OEMVendorNamePtr);

  vci_p->VideoModeList = (uint16_t *) FarPtr_to_Ptr(baseAdd, tempVirt->VideoModeList);

  // free VBE Controller Info Block memory
  if (!lm_free(&lowMem_map))
    return -5;

  return 0;
}

void clear_screen() {

  char *currentPtr = video_mem;

  for (int i = 0; i < hres * vres; i++) {

    *currentPtr = 0;
    currentPtr++;
  }
}

uint16_t *getSegment(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {

  uint16_t *bgImage = (uint16_t *) malloc(sizeof(uint16_t) * width * height);
  uint16_t *result = bgImage;

  // store in currPtr the mem position corresponding to the top left corner of the picture
  uint16_t *currPtr = (uint16_t *) video_mem + (y * hres + x);

  // draw the pixmap
  for (uint16_t i = 0; i < height; i++) {

    currPtr = (uint16_t *) video_mem + ((y + i) * hres + x);

    for (uint16_t j = 0; j < width; j++) {

      *bgImage = *currPtr;

      currPtr++;
      bgImage++;
    }
  }

  return result;
}

int load_background(xpm_string_t *bg) {

  xpm_load(bg, XPM_1_5_5_5, &background);
  return 0;
}

int draw_background() {

  memcpy((char *) scene, (char *) background.bytes, vram_size);
  //vg_draw_rectangle(hres/2-40,vres-81,50,80,0xF0);
  return 0;
}

int draw_background2(xpm_image_t bg) {

  memcpy((char *) scene, (char *) bg.bytes, vram_size);
  
  return 0;
}

int display_scene() {

  memcpy(video_mem, scene, vram_size);

  return 0;
}

char *get_scene() {

  return scene;
}

uint16_t get_hres() {

  return hres;
}

uint16_t get_vres() {

  return vres;
}

xpm_image_t get_background() {

  return background;
}

uint16_t xpm_pixel_at(xpm_image_t xpm, uint16_t x, uint16_t y) {

  uint16_t *currPtr = (uint16_t *) xpm.bytes + (y * xpm.width + x);


  return *currPtr;
}
