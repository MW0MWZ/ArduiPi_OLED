/*********************************************************************
This is a library for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 4 or 5 pins are required to  
interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution

02/18/2013  Charles-Henri Hallard (http://hallard.me)
            Modified for compiling and use on Raspberry ArduiPi Board
            LCD size and connection are now passed as arguments on 
            the command line (no more #define on compilation needed)
            ArduiPi project documentation http://hallard.me/arduipi
            
07/26/2013  Charles-Henri Hallard (http://hallard.me)
            modified name for generic library using different OLED type
 
*********************************************************************/

#ifndef _ArduiPi_OLED_H
#define _ArduiPi_OLED_H

#include "./Adafruit_GFX.h"
#include <pthread.h>

#define BLACK 0
#define WHITE 1


/*=========================================================================
    SSDxxxx Common Displays
    -----------------------------------------------------------------------
    Common values to all displays
=========================================================================*/

//#define SSD_Command_Mode      0x80  /* DC bit is 0 */ Seeed set C0 to 1 why ?
#define SSD_Command_Mode      0x00  /* C0 and DC bit are 0         */
#define SSD_Data_Mode         0x40  /* C0 bit is 0 and DC bit is 1 */

#define SSD_Set_Segment_Remap   0xA0
#define SSD_Inverse_Display     0xA7
#define SSD_Set_Muliplex_Ratio  0xA8

#define SSD_Display_Off         0xAE
#define SSD_Display_On          0xAF

#define SSD_Set_ContrastLevel 0x81

#define SSD_External_Vcc      0x01
#define SSD_Internal_Vcc      0x02

#define SSD_Set_Column_Address  0x21
#define SSD_Set_Page_Address    0x22

#define SSD_Activate_Scroll   0x2F
#define SSD_Deactivate_Scroll 0x2E

#define SSD_Right_Horizontal_Scroll   0x26
#define SSD_Left_Horizontal_Scroll    0x27


#define Scroll_Left           0x00
#define Scroll_Right          0x01

#define Scroll_2Frames    0x07
#define Scroll_3Frames    0x04
#define Scroll_4Frames    0x05
#define Scroll_5Frames    0x00
#define Scroll_25Frames   0x06
#define Scroll_64Frames   0x01
#define Scroll_128Frames  0x02
#define Scroll_256Frames  0x03

#define VERTICAL_MODE           01
#define PAGE_MODE               01
#define HORIZONTAL_MODE         02


/*=========================================================================
    SSD1306 Displays
    -----------------------------------------------------------------------
    The driver is used in multiple displays (128x64, 128x32, etc.).
=========================================================================*/

#define SSD1306_Entire_Display_Resume 0xA4
#define SSD1306_Entire_Display_On     0xA5

#define SSD1306_Normal_Display  0xA6

#define SSD1306_Set_Display_Offset      0xD3
#define SSD1306_Set_Com_Pins        0xDA
#define SSD1306_Set_Vcomh_Deselect_Level      0xDB
#define SSD1306_Set_Display_Clock_Div 0xD5
#define SSD1306_Set_Precharge_Period    0xD9
#define SSD1306_Set_Lower_Column_Start_Address        0x00
#define SSD1306_Set_Higher_Column_Start_Address       0x10
#define SSD1306_Set_Start_Line      0x40
#define SSD1306_Set_Memory_Mode     0x20
#define SSD1306_Set_Com_Output_Scan_Direction_Normal  0xC0
#define SSD1306_Set_Com_Output_Scan_Direction_Remap   0xC8
#define SSD1306_Charge_Pump_Setting 0x8D

// Scrolling #defines
#define SSD1306_SET_VERTICAL_SCROLL_AREA              0xA3
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL  0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL   0x2A

/*=========================================================================
    SSD1308 Displays
    -----------------------------------------------------------------------
    The driver is used in multiple displays (128x64, 128x32, etc.).
=========================================================================*/
#define SSD1308_Normal_Display  0xA6

/*=========================================================================
    SSD1327 Displays
    -----------------------------------------------------------------------
    The driver is used in Seeed 96x96 display
=========================================================================*/
#define SSD1327_Set_Display_Start_Line  0xA1
#define SSD1327_Set_Display_Offset      0xA2
#define SSD1327_Normal_Display      0xA4
#define SSD1327_Set_Display_Clock_Div 0xB3
#define SSD1327_Set_Command_Lock    0xFD
#define SSD1327_Set_Column_Address  0x15
#define SSD1327_Set_Row_Address     0x75

#define SSD1327_Set_Row_Address     0x75

/*=========================================================================
    SH1106 Displays
    -----------------------------------------------------------------------
    The driver is used in multiple displays (128x64, 128x32, etc.).
=========================================================================*/
#define SH1106_Set_Page_Address 0xB0



class ArduiPi_OLED : public Adafruit_GFX 
{
 public:
  ArduiPi_OLED();

  // SPI Init
  boolean init(int8_t DC, int8_t RST, int8_t CS, uint8_t OLED_TYPE);
  
  // I2C Init
  boolean init(int8_t RST, uint8_t OLED_TYPE);

  boolean oled_is_spi_proto(uint8_t OLED_TYPE); /* to know protocol before init */
  boolean select_oled(uint8_t OLED_TYPE) ;
  
  void begin(void);
  void close(void);

  void sendCommand(uint8_t c);
  void sendCommand(uint8_t c0, uint8_t c1);
  void sendCommand(uint8_t c0, uint8_t c1, uint8_t c2);
  void sendCommands(const uint8_t *c, uint8_t len);
  void sendData(uint8_t c);

  void clearDisplay(void);
  void setGrayLevel(uint8_t grayLevel);
  void setBrightness(uint8_t Brightness);
  void invertDisplay(uint8_t i);
  void display();
  
  void setSeedTextXY(unsigned char Row, unsigned char Column);
  void putSeedChar(char C);
  void putSeedString(const char *String);

 
  int16_t getOledWidth(void);
  int16_t getOledHeight(void);

  void startscrollright(uint8_t start, uint8_t stop);
  void startscrollleft(uint8_t start, uint8_t stop);

  void startscrolldiagright(uint8_t start, uint8_t stop);
  void startscrolldiagleft(uint8_t start, uint8_t stop);
  void setHorizontalScrollProperties(bool direction,uint8_t startRow, uint8_t endRow,uint8_t startColumn, uint8_t endColumn, uint8_t scrollSpeed);
  void stopscroll(void);

  void drawPixel(int16_t x, int16_t y, uint16_t color);

  private:
  uint8_t *poledbuff; // Pointer to OLED data buffer in memory
  int8_t _i2c_addr, dc, rst, cs;
  int16_t oled_width, oled_height;
  int16_t oled_buff_size;
  uint8_t vcc_type;
  uint8_t oled_type;
  uint8_t grayH, grayL;
  
  // Hardware scrolling: what the controller can do, and the scroll that
  // is running (replayed around each display(), as the SSD1306 must not
  // have its RAM written while it scrolls)
  boolean isSSD1306Family(void);
  boolean hasHWScroll(void);
  boolean hasDiagScroll(void);
  boolean ch1116; // a type 6 panel whose controller is a CH1116 (see begin())
  uint8_t scrollPage(uint8_t page);
  void sendScroll(void);
  // A scroll's setup: up to 4 commands, each sent in a transaction of its
  // own (scroll_cmdlen: each one's length in scroll_seq)
  uint8_t scroll_seq[24];
  uint8_t scroll_len;
  uint8_t scroll_cmdlen[4];
  uint8_t scroll_ncmd;
  void scrollClear(void);
  void scrollAdd(const uint8_t *c, uint8_t len);
  void startHorizontal(uint8_t cmd, uint8_t start, uint8_t stop);
  void startDiagonal(uint8_t cmd, uint8_t start, uint8_t stop);
  boolean scrolling;

  // What a panel cannot scroll in hardware, the library moves itself, a
  // step at a time from a thread of its own (see startRoll()): the
  // vertical part of a diagonal scroll (roll_v: the display start line,
  // roll_line), and on a panel with no scroll commands the horizontal
  // part too (roll_h: pages roll_start-roll_stop drawn roll_off columns
  // round, roll_dir +1 left or -1 right).
  boolean canRoll(void);
  void startRoll(boolean vertical, boolean horizontal, int8_t dir, uint8_t start, uint8_t stop);
  void stopRoll(void);
  void rollStep(void);
  void writePage(uint8_t page);
  static void *rollThread(void *arg);
  pthread_t roll_thread;
  pthread_mutex_t bus;       // the panel's bus: the roll and frames do not interleave
  pthread_mutex_t roll_lock; // roll_run, with roll_wake
  pthread_cond_t  roll_wake;
  boolean roll_running;      // the thread exists (to be joined)
  boolean roll_run;          // the thread is to keep going
  boolean roll_v, roll_h;
  uint8_t roll_line, roll_off, roll_start, roll_stop;
  int8_t  roll_dir;

  inline boolean isI2C(void);
  inline boolean isSPI(void);
  void fastSPIwrite(uint8_t c);
  void fastSPIwrite(char* tbuf, uint32_t len);
  void fastI2Cwrite(uint8_t c);
  void fastI2Cwrite(char* tbuf, uint32_t len);
  void slowSPIwrite(uint8_t c);




  //volatile uint8_t *dcport;
  //uint8_t dcpinmask;
};
#endif
