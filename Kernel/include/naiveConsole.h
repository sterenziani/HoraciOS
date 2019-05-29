#include <stdint.h>

#define RED_COLOR 0xFF3300
#define BLUE_COLOR 0x3958D0
#define GREEN_COLOR 0x99FF00
#define YELLOW_COLOR 0xFFFF00
#define ORANGE_COLOR 0xFF6600
#define PURPLE_COLOR 0xB139D0
#define WHITE_COLOR 0xFFFFFF
#define BLACK_COLOR 0x000000
#define DEFAULT_FONT_SIZE 1
#define GIANT_FONT_SIZE 15

unsigned char * pixel_map(int c);

#define FONT_COLOR WHITE_COLOR
#define BACKGROUND_COLOR  BLACK_COLOR

void newLine();
void putpixel(unsigned int x, unsigned int y, int color);
void printChar(int c);
void printCharFormat(int c, int color, int bg_color);
char printCharPosition(int c, int color, int bg_color, unsigned int x, unsigned int y);
void fillRect(unsigned int x, unsigned int y, int color, unsigned int w, unsigned int h);
void print(char* string);
void printFormat(char* string, int color, int bg_color);
void printPosition(char* string, int color, int bg_color, unsigned int x, unsigned int y);

void printBigChar(int c, int size);
void printBigCharFormat(int c, int color, int bg_color, int size);
char printBigCharPosition(int c, int color, int bg_color, unsigned int x, unsigned int y, int size);
void printBig(char* string, int size);
void printBigFormat(char* string, int color, int bg_color, int size);
void printBigPosition(char* string, int color, int bg_color, unsigned int x, unsigned int y, int size);
void newBigLine(int size);

void resetCursor();
unsigned int getCursorX();
unsigned int getCursorY();
void setCursor(int x, int y);
void cursorLeft();
void cursorRight();
void eraseLine(int y);
void scrollUp();
void copyLine(int source, int dest);

unsigned int getScreenHeight();
unsigned int getScreenWidth();
void printBin(uint64_t value);
void printBase(uint64_t value, uint32_t base);
void printDec(uint64_t value);
void printHex(uint64_t value);
void clearScreen();

struct vbe_mode_info_structure{
 uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
 uint8_t window_a;			// deprecated
 uint8_t window_b;			// deprecated
 uint16_t granularity;		// deprecated; used while calculating bank numbers
 uint16_t window_size;
 uint16_t segment_a;
 uint16_t segment_b;
 uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
 uint16_t pitch;			// number of bytes per horizontal line
 uint16_t width;			// width in pixels
 uint16_t height;			// height in pixels
 uint8_t w_char;			// unused...
 uint8_t y_char;			// ...
 uint8_t planes;
 uint8_t bpp;			// bits per pixel in this mode
 uint8_t banks;			// deprecated; total number of banks in this mode
 uint8_t memory_model;
 uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
 uint8_t image_pages;
 uint8_t reserved0;

 uint8_t red_mask;
 uint8_t red_position;
 uint8_t green_mask;
 uint8_t green_position;
 uint8_t blue_mask;
 uint8_t blue_position;
 uint8_t reserved_mask;
 uint8_t reserved_position;
 uint8_t direct_color_attributes;

 uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
 uint32_t off_screen_mem_off;
 uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
 uint8_t reserved1[206];
} __attribute__ ((packed));

typedef struct cursor
{
  unsigned int x;
  unsigned int y;
}Cursor;
