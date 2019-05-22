#include <naiveConsole.h>
#include <lib.h>
#include <stdint.h>
#include <font.h>
#include <time.h>

struct vbe_mode_info_structure* screenData = (struct vbe_mode_info_structure*) 0x5C00;

#define LINE_WIDTH screenData->pitch
#define SCREEN_WIDTH screenData->width
#define SCREEN_HEIGHT screenData->height
#define LAST_LINE (SCREEN_HEIGHT - CHAR_HEIGHT)


static Cursor cursor = {1, 1};
static char buffer[64] = { '0' };

// Paints the pixel on (x, y) the specified color
void putpixel(unsigned int x, unsigned int y, int color)
{
    char* screen = (char*)(uint64_t)screenData->framebuffer;
    unsigned where = x*3 + y*LINE_WIDTH;
		screen[where] = color & 255;              // BLUE
    screen[where + 1] = (color >> 8) & 255;   // GREEN
    screen[where + 2] = (color >> 16) & 255;  // RED
}

void printBigChar(int c, int size)
{
  printBigCharFormat(c, FONT_COLOR, BACKGROUND_COLOR, size);
}

void printBigCharFormat(int c, int color, int bg_color, int size)
{
  if(c!=0)
  {
  unsigned char* bitmap = charBitmap(c);
  //check this won't go outside our screen

  //if we're going offscreen, move to next line
  if (cursor.x + CHAR_WIDTH*size >= SCREEN_WIDTH)
  {
    cursor.x = 0;
    if (cursor.y + CHAR_HEIGHT*size >= SCREEN_HEIGHT)
    {
      //Scroll up once for each regular char
      for (int i = 0; i < size; i++)
        scrollUp();
      cursor.y = SCREEN_HEIGHT - CHAR_HEIGHT*size;
    }
    else
      cursor.y += CHAR_HEIGHT*size;
  }

  //if this font fits horizontally but not vertically, make some space
  if (cursor.y + CHAR_HEIGHT*size > SCREEN_HEIGHT)
  {
    //Scroll up once for each regular char
    for (int i = 0; i < size; i++)
      scrollUp();
    cursor.y = SCREEN_HEIGHT - CHAR_HEIGHT*size;
  }

  for(int i = 0; i < CHAR_WIDTH; i++)
  {
    for(int j = 0; j < CHAR_HEIGHT; j++)
    {
      //if current bit equals 1, paint pixel
      if(1<<i & bitmap[j])
        fillRect(cursor.x + (CHAR_WIDTH - i)*size, cursor.y + j*size, color, size, size);
      else
        fillRect(cursor.x + (CHAR_WIDTH - i)*size, cursor.y + j*size, bg_color, size, size);
    }
  }
  cursor.x += CHAR_WIDTH*size;
}
}

void printBig(char* string, int size)
{
  while(*string != 0)
  {
    printBigCharFormat(*string, FONT_COLOR, BACKGROUND_COLOR, size);
    string++;
  }
}

void printBigFormat(char* string, int color, int bg_color, int size)
{
  while(*string != 0)
  {
    printBigCharFormat(*string, color, bg_color, size);
    string++;
  }
}

char printBigCharPosition(int c, int color, int bg_color, unsigned int x, unsigned int y, int size)
{
  if(c!=0)
  {
    char flagY = 0;
    unsigned char* bitmap = charBitmap(c);
    //check this won't go outside our screen
    if (x + CHAR_WIDTH*size >= SCREEN_WIDTH)
    {
      x = 0;
      if (y + CHAR_HEIGHT*size >= SCREEN_HEIGHT)
      {
        for (int i = 0; i < size; i++)
          scrollUp();
        y = SCREEN_HEIGHT - CHAR_HEIGHT*size;
        flagY = 1;
      }
      else
        y += CHAR_HEIGHT*size;
    }
    for(int i = 0; i < CHAR_WIDTH*size; i++)
    {
      for(int j = 0; j < CHAR_HEIGHT*size; j++)
      {
        //if current bit equals 1, paint pixel
        if(1<<i & bitmap[j])
          fillRect(cursor.x + CHAR_WIDTH*size - i, cursor.y + j, color, size, size);
        else
          fillRect(cursor.x + CHAR_WIDTH*size - i, cursor.y +j, bg_color, size, size);
      }
    }
    x += CHAR_WIDTH*size;
    return flagY;
  }
  return 0;
}

void printBigPosition(char* string, int color, int bg_color, unsigned int x, unsigned int y, int size)
{
  int flag = 0;
  while(*string != 0)
  {
    flag = printBigCharPosition(*string, color, bg_color, x, y, size);
    x += CHAR_WIDTH*size;
    if(flag == 1)
    {
      y+=CHAR_HEIGHT*size;
      x-=CHAR_WIDTH*size;
      flag = 0;
    }
  }
}

// Prints char where cursor points to, using default colors
void printChar(int c)
{
  printCharFormat(c, FONT_COLOR, BACKGROUND_COLOR);
}

// Prints char where cursor points to, using specific colors
void printCharFormat(int c, int color, int bg_color)
{
  if(c!=0)
  {
    unsigned char* bitmap = charBitmap(c);
    //check this won't go outside our screen
    if (cursor.x + CHAR_WIDTH >= SCREEN_WIDTH)
    {
      cursor.x = 0;
      if (cursor.y + CHAR_HEIGHT >= SCREEN_HEIGHT)
      {
        scrollUp();
        cursor.y = SCREEN_HEIGHT - CHAR_HEIGHT;
      }
      else
        cursor.y += CHAR_HEIGHT;
    }
    for(int i = 0; i < CHAR_WIDTH; i++)
    {
      for(int j = 0; j < CHAR_HEIGHT; j++)
      {
        //if current bit equals 1, paint pixel
        if(1<<i & bitmap[j])
          putpixel(cursor.x + CHAR_WIDTH - i, cursor.y + j, color);
        else
          putpixel(cursor.x + CHAR_WIDTH - i, cursor.y +j, bg_color);
      }
    }
    cursor.x += CHAR_WIDTH;
  }
}

// Prints string starting from a certain position. Returns 1 if Y was modified.
char printCharPosition(int c, int color, int bg_color, unsigned int x, unsigned int y)
{
  if(c!=0)
  {
    char flagY = 0;
    unsigned char* bitmap = charBitmap(c);
    //check this won't go outside our screen
    if (x + CHAR_WIDTH >= SCREEN_WIDTH)
    {
      x = 0;
      if (y + CHAR_HEIGHT >= SCREEN_HEIGHT)
      {
        scrollUp();
        y = SCREEN_HEIGHT - CHAR_HEIGHT;
        flagY = 1;
      }
      else
        y += CHAR_HEIGHT;
    }
    for(int i = 0; i < CHAR_WIDTH; i++)
    {
      for(int j = 0; j < CHAR_HEIGHT; j++)
      {
        //if current bit equals 1, paint pixel
        if(1<<i & bitmap[j])
          putpixel(x + CHAR_WIDTH - i, y + j, color);
        else
          putpixel(x + CHAR_WIDTH - i, y +j, bg_color);
      }
    }
    x += CHAR_WIDTH;
    return flagY;
  }
  return 0;
}

// Draws a (W x H) rectangle of the specified color, starting from (x,y)
void fillRect(unsigned int x, unsigned int y, int color, unsigned int w, unsigned int h)
{
	for (int i = x; i < x+w; i++)
		for (int j = y; j < y+h; j++)
			putpixel(i, j, color);
}

void print(char* string)
{
  while(*string != 0)
  {
    printCharFormat(*string, FONT_COLOR, BACKGROUND_COLOR);
    string++;
  }
}

// Prints string where cursor points to, using specific colors
void printFormat(char* string, int color, int bg_color)
{
  while(*string != 0)
  {
    printCharFormat(*string, color, bg_color);
    string++;
  }
}

// Prints string starting from a certain position
void printPosition(char* string, int color, int bg_color, unsigned int x, unsigned int y)
{
  int flag = 0;
  while(*string != 0)
  {
    flag = printCharPosition(*string, color, bg_color, x, y);
    x += CHAR_WIDTH;
    if(flag == 1)
    {
      y+=CHAR_HEIGHT;
      x-=CHAR_WIDTH;
      flag = 0;
    }
  }
}

unsigned int getScreenWidth()
{
  return SCREEN_WIDTH;
}

unsigned int getScreenHeight()
{
  return SCREEN_HEIGHT;
}

void scrollUp()
{
  for(int y = CHAR_HEIGHT; y <= LAST_LINE; y += CHAR_HEIGHT)
     copyLine(y, y-CHAR_HEIGHT);
  eraseLine(LAST_LINE);
}

void eraseLine(int y)
{
  for (int i = 0; i < SCREEN_WIDTH-CHAR_WIDTH; i+=CHAR_WIDTH)
  {
      printCharPosition(' ', BACKGROUND_COLOR, BACKGROUND_COLOR, i, y);
  }
}

void copyLine(int source, int dest)
{
  char* sourceLine = (char*)(uint64_t)screenData->framebuffer + LINE_WIDTH*source;
  char* destLine = (char*)(uint64_t)screenData->framebuffer + LINE_WIDTH*dest;
  int x, y;
  for(y = 0; y < 3*CHAR_HEIGHT; y++)
  {
     for(x = 0 ; x < SCREEN_WIDTH; x++)
     {
        destLine[0] = sourceLine[0];
        destLine[1] = sourceLine[1];
        destLine[2] = sourceLine[2];
        destLine+=1;
        sourceLine+=1;
    }
  }
}

// Jumps to the line below
void newLine()
{
  int originalLine = cursor.y;
  //if not last line, print until we go down
  if(cursor.y < LAST_LINE)
  {
    while(cursor.y == originalLine)
      cursorRight();
    cursor.x -= CHAR_WIDTH;
  }
  else
  {
    scrollUp();
    cursor.x = 0;
    cursor.y = LAST_LINE;
  }
}

void newBigLine(int size)
{
  for (int i = 0; i < size; i++)
    newLine();
}

void clearScreen()
{
  fillRect(0, 0, BACKGROUND_COLOR, SCREEN_WIDTH, SCREEN_HEIGHT);
  cursor.x = 0;
  cursor.y = 0;
}

void resetCursor()
{
  setCursor(0,0);
}

void setCursor(int x, int y)
{
  cursor.x = x;
  cursor.y = y;
}

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
	return digits;
}

void printBase(uint64_t value, uint32_t base)
{
    uintToBase(value, buffer, base);
    print(buffer);
}

void printDec(uint64_t value)
{
	printBase(value, 10);
}

void printHex(uint64_t value)
{
	printBase(value, 16);
}

void printBin(uint64_t value)
{
	printBase(value, 2);
}

void cursorLeft()
{
  if(cursor.x < CHAR_WIDTH && cursor.y != 0)
  {
      cursor.y -= CHAR_HEIGHT;
      cursor.x = SCREEN_WIDTH - (SCREEN_WIDTH%CHAR_WIDTH) - 2*CHAR_WIDTH;
  }
  else if (cursor.x >= CHAR_WIDTH)
    cursor.x -= CHAR_WIDTH;
}

void cursorRight()
{
  if (cursor.x + CHAR_WIDTH >= SCREEN_WIDTH)
  {
    cursor.x = 0;
    if (cursor.y + CHAR_HEIGHT >= SCREEN_HEIGHT)
    {
      scrollUp();
      cursor.y = SCREEN_HEIGHT - CHAR_HEIGHT;
    }
    else
      cursor.y += CHAR_HEIGHT;
  }
  cursor.x += CHAR_WIDTH;
}

unsigned int getCursorX()
{
  return cursor.x;
}

unsigned int getCursorY()
{
  return cursor.y;
}
