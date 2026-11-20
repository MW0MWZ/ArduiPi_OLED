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
All text above, and the splash screen below must be included in any redistribution

02/18/2013  Charles-Henri Hallard (http://hallard.me)
            Modified for compiling and use on Raspberry ArduiPi Board
            LCD size and connection are now passed as arguments on 
            the command line (no more #define on compilation needed)
            ArduiPi project documentation http://hallard.me/arduipi
07/01/2013  Charles-Henri Hallard 
            Reduced code size removed the Adafruit Logo (sorry guys)
            Buffer for OLED is now dynamic to LCD size
            Added support of Seeed OLED 64x64 Display
            
07/26/2013  Charles-Henri Hallard 
            modified name for generic library using different OLED type

02/24/2015  Charles-Henri Hallard 
            added support for 1.3" I2C OLED with SH1106 driver

11/20/2026  Andy Taylor (MW0MWZ)
            display() sets the SSD1306 write window for every frame, so
            a byte lost on the bus no longer shifts every frame after it.
            Hardware scrolling follows the SSD1306 datasheet: stopped for
            each frame (no RAM writes while it scrolls) and before its
            settings change, pages clamped to the display's own, dummy
            byte 00h; none started on the SH1106 and SSD1327, which have
            no scroll commands, but stopscroll() still stops one on
            every type. A type 6 panel whose controller is a CH1116 (its
            status byte's ID, 010110) scrolls with the CH1116's own
            command layout, over its 128 visible columns. On the
            SSD1306 a horizontal scroll clears the vertical scroll area
            a diagonal one left (A3h 00h 00h): it rolled the whole screen
            up during calls, and clears the vertical offset it left
            (a vertical-and-horizontal setup with offset 0). A scroll's
            setup is sent in one I2C transaction per command: some
            compatibles take a command only that way. The diagonal
            scroll is set up for the SSD1315 too (its 7-byte layout,
            horizontal scroll on), which the SSD1306 reads as its own.
            
*********************************************************************/

#include "./ArduiPi_OLED_lib.h" 
#include "./Adafruit_GFX.h"
#include "./ArduiPi_OLED.h"

  // 8x8 Font ASCII 32 - 127 Implemented
// Users can modify this to support more characters(glyphs)
// BasicFont is placed in code memory.

// This font can be freely used without any restriction(It is placed in public domain)
const unsigned char seedfont[][8] =
{
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x00,0x5F,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x00,0x07,0x00,0x07,0x00,0x00,0x00},
  {0x00,0x14,0x7F,0x14,0x7F,0x14,0x00,0x00},
  {0x00,0x24,0x2A,0x7F,0x2A,0x12,0x00,0x00},
  {0x00,0x23,0x13,0x08,0x64,0x62,0x00,0x00},
  {0x00,0x36,0x49,0x55,0x22,0x50,0x00,0x00},
  {0x00,0x00,0x05,0x03,0x00,0x00,0x00,0x00},
  {0x00,0x1C,0x22,0x41,0x00,0x00,0x00,0x00},
  {0x00,0x41,0x22,0x1C,0x00,0x00,0x00,0x00},
  {0x00,0x08,0x2A,0x1C,0x2A,0x08,0x00,0x00},
  {0x00,0x08,0x08,0x3E,0x08,0x08,0x00,0x00},
  {0x00,0xA0,0x60,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x08,0x08,0x08,0x08,0x08,0x00,0x00},
  {0x00,0x60,0x60,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x20,0x10,0x08,0x04,0x02,0x00,0x00},
  {0x00,0x3E,0x51,0x49,0x45,0x3E,0x00,0x00},
  {0x00,0x00,0x42,0x7F,0x40,0x00,0x00,0x00},
  {0x00,0x62,0x51,0x49,0x49,0x46,0x00,0x00},
  {0x00,0x22,0x41,0x49,0x49,0x36,0x00,0x00},
  {0x00,0x18,0x14,0x12,0x7F,0x10,0x00,0x00},
  {0x00,0x27,0x45,0x45,0x45,0x39,0x00,0x00},
  {0x00,0x3C,0x4A,0x49,0x49,0x30,0x00,0x00},
  {0x00,0x01,0x71,0x09,0x05,0x03,0x00,0x00},
  {0x00,0x36,0x49,0x49,0x49,0x36,0x00,0x00},
  {0x00,0x06,0x49,0x49,0x29,0x1E,0x00,0x00},
  {0x00,0x00,0x36,0x36,0x00,0x00,0x00,0x00},
  {0x00,0x00,0xAC,0x6C,0x00,0x00,0x00,0x00},
  {0x00,0x08,0x14,0x22,0x41,0x00,0x00,0x00},
  {0x00,0x14,0x14,0x14,0x14,0x14,0x00,0x00},
  {0x00,0x41,0x22,0x14,0x08,0x00,0x00,0x00},
  {0x00,0x02,0x01,0x51,0x09,0x06,0x00,0x00},
  {0x00,0x32,0x49,0x79,0x41,0x3E,0x00,0x00},
  {0x00,0x7E,0x09,0x09,0x09,0x7E,0x00,0x00},
  {0x00,0x7F,0x49,0x49,0x49,0x36,0x00,0x00},
  {0x00,0x3E,0x41,0x41,0x41,0x22,0x00,0x00},
  {0x00,0x7F,0x41,0x41,0x22,0x1C,0x00,0x00},
  {0x00,0x7F,0x49,0x49,0x49,0x41,0x00,0x00},
  {0x00,0x7F,0x09,0x09,0x09,0x01,0x00,0x00},
  {0x00,0x3E,0x41,0x41,0x51,0x72,0x00,0x00},
  {0x00,0x7F,0x08,0x08,0x08,0x7F,0x00,0x00},
  {0x00,0x41,0x7F,0x41,0x00,0x00,0x00,0x00},
  {0x00,0x20,0x40,0x41,0x3F,0x01,0x00,0x00},
  {0x00,0x7F,0x08,0x14,0x22,0x41,0x00,0x00},
  {0x00,0x7F,0x40,0x40,0x40,0x40,0x00,0x00},
  {0x00,0x7F,0x02,0x0C,0x02,0x7F,0x00,0x00},
  {0x00,0x7F,0x04,0x08,0x10,0x7F,0x00,0x00},
  {0x00,0x3E,0x41,0x41,0x41,0x3E,0x00,0x00},
  {0x00,0x7F,0x09,0x09,0x09,0x06,0x00,0x00},
  {0x00,0x3E,0x41,0x51,0x21,0x5E,0x00,0x00},
  {0x00,0x7F,0x09,0x19,0x29,0x46,0x00,0x00},
  {0x00,0x26,0x49,0x49,0x49,0x32,0x00,0x00},
  {0x00,0x01,0x01,0x7F,0x01,0x01,0x00,0x00},
  {0x00,0x3F,0x40,0x40,0x40,0x3F,0x00,0x00},
  {0x00,0x1F,0x20,0x40,0x20,0x1F,0x00,0x00},
  {0x00,0x3F,0x40,0x38,0x40,0x3F,0x00,0x00},
  {0x00,0x63,0x14,0x08,0x14,0x63,0x00,0x00},
  {0x00,0x03,0x04,0x78,0x04,0x03,0x00,0x00},
  {0x00,0x61,0x51,0x49,0x45,0x43,0x00,0x00},
  {0x00,0x7F,0x41,0x41,0x00,0x00,0x00,0x00},
  {0x00,0x02,0x04,0x08,0x10,0x20,0x00,0x00},
  {0x00,0x41,0x41,0x7F,0x00,0x00,0x00,0x00},
  {0x00,0x04,0x02,0x01,0x02,0x04,0x00,0x00},
  {0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00},
  {0x00,0x01,0x02,0x04,0x00,0x00,0x00,0x00},
  {0x00,0x20,0x54,0x54,0x54,0x78,0x00,0x00},
  {0x00,0x7F,0x48,0x44,0x44,0x38,0x00,0x00},
  {0x00,0x38,0x44,0x44,0x28,0x00,0x00,0x00},
  {0x00,0x38,0x44,0x44,0x48,0x7F,0x00,0x00},
  {0x00,0x38,0x54,0x54,0x54,0x18,0x00,0x00},
  {0x00,0x08,0x7E,0x09,0x02,0x00,0x00,0x00},
  {0x00,0x18,0xA4,0xA4,0xA4,0x7C,0x00,0x00},
  {0x00,0x7F,0x08,0x04,0x04,0x78,0x00,0x00},
  {0x00,0x00,0x7D,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x80,0x84,0x7D,0x00,0x00,0x00,0x00},
  {0x00,0x7F,0x10,0x28,0x44,0x00,0x00,0x00},
  {0x00,0x41,0x7F,0x40,0x00,0x00,0x00,0x00},
  {0x00,0x7C,0x04,0x18,0x04,0x78,0x00,0x00},
  {0x00,0x7C,0x08,0x04,0x7C,0x00,0x00,0x00},
  {0x00,0x38,0x44,0x44,0x38,0x00,0x00,0x00},
  {0x00,0xFC,0x24,0x24,0x18,0x00,0x00,0x00},
  {0x00,0x18,0x24,0x24,0xFC,0x00,0x00,0x00},
  {0x00,0x00,0x7C,0x08,0x04,0x00,0x00,0x00},
  {0x00,0x48,0x54,0x54,0x24,0x00,0x00,0x00},
  {0x00,0x04,0x7F,0x44,0x00,0x00,0x00,0x00},
  {0x00,0x3C,0x40,0x40,0x7C,0x00,0x00,0x00},
  {0x00,0x1C,0x20,0x40,0x20,0x1C,0x00,0x00},
  {0x00,0x3C,0x40,0x30,0x40,0x3C,0x00,0x00},
  {0x00,0x44,0x28,0x10,0x28,0x44,0x00,0x00},
  {0x00,0x1C,0xA0,0xA0,0x7C,0x00,0x00,0x00},
  {0x00,0x44,0x64,0x54,0x4C,0x44,0x00,0x00},
  {0x00,0x08,0x36,0x41,0x00,0x00,0x00,0x00},
  {0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x41,0x36,0x08,0x00,0x00,0x00,0x00},
  {0x00,0x02,0x01,0x01,0x02,0x01,0x00,0x00},
  {0x00,0x02,0x05,0x05,0x02,0x00,0x00,0x00} 
};


inline boolean ArduiPi_OLED::isSPI(void) {
  return (cs != -1 ? true : false);
}
inline boolean ArduiPi_OLED::isI2C(void) {
  return (cs == -1 ? true : false);
}
// Low level I2C and SPI Write function
inline void ArduiPi_OLED::fastSPIwrite(uint8_t d) {
  bcm2835_spi_transfer(d);
}
inline void ArduiPi_OLED::fastI2Cwrite(uint8_t d) {
  bcm2835_spi_transfer(d);
}
inline void ArduiPi_OLED::fastSPIwrite(char* tbuf, uint32_t len) {
  bcm2835_spi_writenb(tbuf, len);
}
inline void ArduiPi_OLED::fastI2Cwrite(char* tbuf, uint32_t len) {
  bcm2835_i2c_write(tbuf, len);
}

// the most basic function, set a single pixel
void ArduiPi_OLED::drawPixel(int16_t x, int16_t y, uint16_t color) 
{
  uint8_t * p = poledbuff ;
  uint8_t c;
  
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

    /*
    // check rotation, move pixel around if necessary
  switch (getRotation()) 
  {
    case 1:
      swap(x, y);
      x = WIDTH - x - 1;
    break;
    
    case 2:
      x = WIDTH - x - 1;
      y = HEIGHT - y - 1;
    break;
    
    case 3:
      swap(x, y);
      y = HEIGHT - y - 1;
    break;
  }  
*/
  if (oled_type == OLED_SEEED_I2C_96x96 )
  {
    // Get where to do the change in the buffer
    p = poledbuff + (x + (y/2)*oled_width );
    
    // Get old value to not touch the other nible
    c = *p;
    
    // We are on High nible ?
    if ( ((y/2)&1) == 1 )
    {
      c &= 0x0F;
      c|= (color==WHITE ? grayH:0x00) << 4;
    } 
    else
    {
      c &= 0xF0;
      c|= (color==WHITE ? grayL:0x00) ;
    } 
    
    // set new nible value leaving the other untouched
    *p = c; 
  }
  else
  {
    // Get where to do the change in the buffer
    p = poledbuff + (x + (y/8)*oled_width );
    
    // x is which column
    if (color == WHITE) 
      *p |=  _BV((y%8));  
    else
      *p &= ~_BV((y%8)); 
  }
}

// Display instantiation
ArduiPi_OLED::ArduiPi_OLED() 
{
  // Init all var, and clean
  // Command I/O
  rst = 0 ;
  dc  = 0 ;
  cs =  0 ;

  // No hardware scroll running
  scrolling   = false;
  scroll_len  = 0;
  scroll_ncmd = 0;
  ch1116     = false;
  
  // Lcd size
  oled_width  = 0;
  oled_height = 0;
  
  // Empty pointer to OLED buffer
  poledbuff = NULL;
}


// When not initialized program using this library may
// know protocol for correct init call, he could just know
// oled number in driver list
boolean ArduiPi_OLED::oled_is_spi_proto(uint8_t OLED_TYPE) 
{
  switch (OLED_TYPE)
  {
    case OLED_ADAFRUIT_SPI_128x32:
    case OLED_ADAFRUIT_SPI_128x64:
      return true;
    break;
  }
    
  // default 
  return false;
  
}

// initializer for OLED Type
boolean ArduiPi_OLED::select_oled(uint8_t OLED_TYPE) 
{
  // Default type
  oled_width  = 128;
  oled_height = 64;
  _i2c_addr = 0x00;
  oled_type = OLED_TYPE;
  
  // default OLED are using internal boost VCC converter
  vcc_type = SSD_Internal_Vcc;

  // Oled supported display
  // Setup size and I2C address
  switch (OLED_TYPE)
  {
    case OLED_ADAFRUIT_SPI_128x32:
      oled_height = 32;
    break;

    case OLED_ADAFRUIT_SPI_128x64:
    ;
    break;
    
    case OLED_ADAFRUIT_I2C_128x32:
      oled_height = 32;
      _i2c_addr = ADAFRUIT_I2C_ADDRESS;
    break;

    case OLED_ADAFRUIT_I2C_128x64:
      _i2c_addr = ADAFRUIT_I2C_ADDRESS;
    break;
    
    case OLED_SEEED_I2C_128x64:
      _i2c_addr = SEEED_I2C_ADDRESS ;
      vcc_type = SSD_External_Vcc;
    break;

    case OLED_SEEED_I2C_96x96:
      oled_width  = 96;
      oled_height = 96;
      _i2c_addr = SEEED_I2C_ADDRESS ;
    break;
    
    case OLED_SH1106_I2C_128x64:
      _i2c_addr = SH1106_I2C_ADDRESS;
    break;
    
    // houston, we have a problem
    default:
      return false;
    break;
  }
  
  // Buffer size differ from OLED type, 1 pixel is one bit 
  // execpt for 96x96 seed, 1 pixel is 1 nible
  oled_buff_size = oled_width * oled_height ;
  
  if ( OLED_TYPE == OLED_SEEED_I2C_96x96 )
    oled_buff_size = oled_buff_size / 2 ;
  else
    oled_buff_size = oled_buff_size / 8;

  // De-Allocate memory for OLED buffer if any
  if (poledbuff)
    free(poledbuff);
    
  // Allocate memory for OLED buffer
  poledbuff = (uint8_t *) malloc ( oled_buff_size ); 
  
  if (!poledbuff)
    return false;

  // Init Raspberry PI GPIO
  if (!bcm2835_init())
    return false;
    
  return true;
  
}

// initializer for SPI - we indicate the pins used and OLED type
//
boolean ArduiPi_OLED::init(int8_t DC, int8_t RST, int8_t CS, uint8_t OLED_TYPE) 
{
  rst = RST;  // Reset Pin
  dc = DC;    // Data / command Pin
  cs = CS ;   // Raspberry SPI chip Enable (may be CE0 or CE1)
  
  // Select OLED parameters
  if (!select_oled(OLED_TYPE))
    return false;

  // Init & Configure Raspberry PI SPI
  bcm2835_spi_begin(cs);
  bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                
  
  // 16 MHz SPI bus, but Worked at 62 MHz also  
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_16); 

  // Set the pin that will control DC as output
  bcm2835_gpio_fsel(dc, BCM2835_GPIO_FSEL_OUTP);

  // Setup reset pin direction as output
  bcm2835_gpio_fsel(rst, BCM2835_GPIO_FSEL_OUTP);

  return ( true);
}

// initializer for I2C - we only indicate the reset pin and OLED type !
boolean ArduiPi_OLED::init(int8_t RST, uint8_t OLED_TYPE) 
{
  dc = cs = -1; // DC and chip Select do not exist in I2C
  rst = RST;

  // Select OLED parameters
  if (!select_oled(OLED_TYPE))
    return false;

  // Init & Configure Raspberry PI I2C
  if (bcm2835_i2c_begin()==0)
    return false;
    
  bcm2835_i2c_setSlaveAddress(_i2c_addr) ;
    
  // Set clock to 400 KHz
  // does not seem to work, will check this later
  // bcm2835_i2c_set_baudrate(400000);

  // Setup reset pin direction as output
  bcm2835_gpio_fsel(rst, BCM2835_GPIO_FSEL_OUTP);
  
  return ( true);
}

void ArduiPi_OLED::close(void) 
{
  // De-Allocate memory for OLED buffer if any
  if (poledbuff)
    free(poledbuff);
    
  poledbuff = NULL;

  // Release Raspberry SPI
  if ( isSPI() )
    bcm2835_spi_end();

    // Release Raspberry I2C
  if ( isI2C() )
    bcm2835_i2c_end();

  // Release Raspberry I/O control
  bcm2835_close();
}

  
void ArduiPi_OLED::begin( void ) 
{
  uint8_t multiplex;
  uint8_t chargepump;
  uint8_t compins;
  uint8_t contrast;
  uint8_t precharge;
  
  constructor(oled_width, oled_height);

  // Setup reset pin direction (used by both SPI and I2C)  
  bcm2835_gpio_fsel(rst, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_write(rst, HIGH);
  
  // VDD (3.3V) goes high at start, lets just chill for a ms
  usleep(1000);
  
  // bring reset low
  bcm2835_gpio_write(rst, LOW);
  
  // wait 10ms
  usleep(10000);
  
  // bring out of reset
  bcm2835_gpio_write(rst, HIGH);
  
  // depends on OLED type configuration
  if (oled_height == 32)
  {
    multiplex = 0x1F;
    compins   = 0x02;
    contrast  = 0x8F;
  }
  else
  {
    if (oled_type == OLED_SEEED_I2C_96x96 )
    {
      multiplex = 0x5F;
      compins   = 0x12;
      contrast  = 0x53;
    }
    // So 128x64
    else
    {
      multiplex = 0x3F;
      compins   = 0x12;
      
      if (oled_type == OLED_SH1106_I2C_128x64)
        contrast = 0x80;
      else
        contrast = (vcc_type==SSD_External_Vcc?0x9F:0xCF);
    }
      
  }
  
  if (vcc_type == SSD_External_Vcc)
  {
    chargepump = 0x10; 
    precharge  = 0x22;
  }
  else
  {
    chargepump = 0x14; 
    precharge  = 0xF1;
  }
  
  if (oled_type == OLED_SEEED_I2C_96x96 )
    sendCommand(SSD1327_Set_Command_Lock, 0x12); // Unlock OLED driver IC MCU interface from entering command. i.e: Accept commands
  
  sendCommand(SSD_Display_Off);                    
  sendCommand(SSD_Set_Muliplex_Ratio, multiplex); 
  
  if (oled_type == OLED_SEEED_I2C_96x96 )
  {
    sendCommand(SSD1327_Set_Display_Clock_Div, 0x01); 
    sendCommand(SSD1327_Set_Display_Start_Line    , 0   ); 
    sendCommand(SSD1327_Set_Display_Offset, 96  ); 
    sendCommand(SSD_Set_Segment_Remap     , 0x46); 

    sendCommand(0xAB); // set vdd internal
    sendCommand(0x01); //
    
    sendCommand(0xB1); // Set Phase Length
    sendCommand(0X51); //
    
    sendCommand(0xB9); //
    
    sendCommand(0xBC); // set pre_charge voltage/VCOMH
    sendCommand(0x08); // (0x08);
    
    sendCommand(0xBE); // set VCOMH
    sendCommand(0X07); // (0x07);
    
    sendCommand(0xB6); // Set second pre-charge period
    sendCommand(0x01); //
    
    sendCommand(0xD5); // enable second precharge and enternal vsl
    sendCommand(0X62); // (0x62);

    // Set Normal Display Mode
    sendCommand(SSD1327_Normal_Display); 

    // Row Address
    // Start 0 End 95
    sendCommand(SSD1327_Set_Row_Address, 0, 95);

    // Column Address
    // Start from 8th Column of driver IC. This is 0th Column for OLED 
    // End at  (8 + 47)th column. Each Column has 2 pixels(segments)
    sendCommand(SSD1327_Set_Column_Address, 8, 0x37 );  

    // Map to horizontal mode
    sendCommand(0xA0); // remap to
    sendCommand(0x46); // Vertical mode

    // Init gray level for text. Default:Brightest White
    grayH= 0xF0;
    grayL= 0x0F;
  }
  else if (oled_type == OLED_SH1106_I2C_128x64)
  {
    sendCommand(SSD1306_Set_Lower_Column_Start_Address|0x02); /*set lower column address*/
    sendCommand(SSD1306_Set_Higher_Column_Start_Address);     /*set higher column address*/
    sendCommand(SSD1306_Set_Start_Line);                      /*set display start line*/
    sendCommand(SH1106_Set_Page_Address);    /*set page address*/
    sendCommand(SSD_Set_Segment_Remap|0x01); /*set segment remap*/
    sendCommand(SSD1306_Normal_Display);     /*normal / reverse*/
    sendCommand(0xad);    /*set charge pump enable*/
    sendCommand(0x8b);    /*external VCC   */
    sendCommand(0x30);    /*0X30---0X33  set VPP   9V liangdu!!!!*/
    sendCommand(SSD1306_Set_Com_Output_Scan_Direction_Remap);    /*Com scan direction*/
    sendCommand(SSD1306_Set_Display_Offset);    /*set display offset*/
    sendCommand(0x00);   /*   0x20  */
    sendCommand(SSD1306_Set_Display_Clock_Div);    /*set osc division*/
    sendCommand(0x80);
    sendCommand(SSD1306_Set_Precharge_Period);    /*set pre-charge period*/
    sendCommand(0x1f);    /*0x22*/
    sendCommand(SSD1306_Set_Com_Pins);    /*set COM pins*/
    sendCommand(0x12);
    sendCommand(SSD1306_Set_Vcomh_Deselect_Level);    /*set vcomh*/
    sendCommand(0x40);
  }
  else
  {
    sendCommand(SSD1306_Charge_Pump_Setting, chargepump); 
    sendCommand(SSD1306_Set_Memory_Mode, 0x00);              // 0x20 0x0 act like ks0108
    sendCommand(SSD1306_Set_Display_Clock_Div, 0x80);      // 0xD5 + the suggested ratio 0x80
    sendCommand(SSD1306_Set_Display_Offset, 0x00);        // no offset
    sendCommand(SSD1306_Set_Start_Line | 0x0);            // line #0
    // use this two commands to flip display
    sendCommand(SSD_Set_Segment_Remap | 0x1);
    sendCommand(SSD1306_Set_Com_Output_Scan_Direction_Remap);
    
    sendCommand(SSD1306_Set_Com_Pins, compins);  
    sendCommand(SSD1306_Set_Precharge_Period, precharge); 
    sendCommand(SSD1306_Set_Vcomh_Deselect_Level, 0x40); // 0x40 -> unknown value in datasheet
    sendCommand(SSD1306_Entire_Display_Resume);    
    sendCommand(SSD1306_Normal_Display);         // 0xA6

    // Reset to default value in case of 
    // no reset pin available on OLED, 
    sendCommand( SSD_Set_Column_Address, 0, 127 ); 
    sendCommand( SSD_Set_Page_Address, 0,   7 ); 
  }

  sendCommand(SSD_Set_ContrastLevel, contrast);

  // Boards sold as SH1106 may carry a CH1116, which also scrolls in
  // hardware, with its own command layout. Its status byte names it: the
  // low six bits are its ID, 010110 (CH1116 datasheet, Read Status); a
  // genuine SH1106 has no ID bits there.
  ch1116 = false;
  if (oled_type == OLED_SH1106_I2C_128x64 && isI2C())
  {
    int status = bcm2835_i2c_read_status();
    ch1116 = status >= 0 && (status & 0x3F) == 0x16;
  }

  stopscroll();
  
  // Empty uninitialized buffer
  clearDisplay();
  
  // turn on oled panel
  sendCommand(SSD_Display_On);              
  
  // wait 100ms
  usleep(100000);
}

// Only valid for Seeed 96x96 OLED
void ArduiPi_OLED::setGrayLevel(uint8_t grayLevel)
{
    grayH = (grayLevel << 4) & 0xF0;
    grayL =  grayLevel & 0x0F;
}

void ArduiPi_OLED::setSeedTextXY(unsigned char Row, unsigned char Column)
{
    //Column Address
    sendCommand(0x15);             /* Set Column Address */
    sendCommand(0x08+(Column*4));  /* Start Column: Start from 8 */
    sendCommand(0x37);             /* End Column */
    // Row Address
    sendCommand(0x75);             /* Set Row Address */
    sendCommand(0x00+(Row*8));     /* Start Row*/
    sendCommand(0x07+(Row*8));     /* End Row*/
}

void ArduiPi_OLED::putSeedChar(char C)
{
    if(C < 32 || C > 127) //Ignore non-printable ASCII characters. This can be modified for multilingual font.
    {
        C=' '; //Space
    } 

    for(char i=0;i<8;i=i+2)
    {
        for(char j=0;j<8;j++)
        {
            // Character is constructed two pixel at a time using vertical mode from the default 8x8 font
            char c=0x00;
            char bit1=( seedfont[C-32][i]   >> j) & 0x01;  
            char bit2=( seedfont[C-32][i+1] >> j) & 0x01;
           // Each bit is changed to a nibble
            c|=(bit1)?grayH:0x00;
            c|=(bit2)?grayL:0x00;
            sendData(c);
        }
    }
}

void ArduiPi_OLED::putSeedString(const char *String)
{
    unsigned char i=0;
    while(String[i])
    {
        putSeedChar( String[i]);     
        i++;
    }
}

void ArduiPi_OLED::setBrightness(uint8_t Brightness)
{
   sendCommand(SSD_Set_ContrastLevel);
   sendCommand(Brightness);
}


void ArduiPi_OLED::invertDisplay(uint8_t i) 
{
  if (i) 
    sendCommand(SSD_Inverse_Display);
  else 
    sendCommand(oled_type==OLED_SEEED_I2C_96x96 ? SSD1327_Normal_Display : SSD1306_Normal_Display);
}

void ArduiPi_OLED::sendCommand(uint8_t c) 
{ 
  // Is SPI
  if (isSPI())
  {
    // Setup D/C line to low to switch to command mode
    bcm2835_gpio_write(dc, LOW);

    // Write Data on SPI
    fastSPIwrite(c);
  }
  // so I2C
  else
  {
    char buff[2] ;
    
    // Clear D/C to switch to command mode
    buff[0] = SSD_Command_Mode ; 
    buff[1] = c;
    
    // Write Data on I2C
    fastI2Cwrite(buff, sizeof(buff))  ;
  }
}

// sendCommands sends a command with its parameters (or several) in one
// transfer: on I2C one transaction, control byte 00h then the bytes.
void ArduiPi_OLED::sendCommands(const uint8_t *c, uint8_t len)
{
  if (isSPI())
  {
    bcm2835_gpio_write(dc, LOW);
    for (uint8_t i = 0; i < len; i++)
      fastSPIwrite(c[i]);
  }
  else
  {
    char buff[33];
    if (len > 32)
      len = 32;
    buff[0] = SSD_Command_Mode;
    memcpy(&buff[1], c, len);
    fastI2Cwrite(buff, len + 1);
  }
}

void ArduiPi_OLED::sendCommand(uint8_t c0, uint8_t c1) 
{ 
  char buff[3] ;
  buff[1] = c0;
  buff[2] = c1;

  // Is SPI
  if (isSPI())
  {
    // Setup D/C line to low to switch to command mode
    bcm2835_gpio_write(dc, LOW);

    // Write Data
    fastSPIwrite(&buff[1], 2);
  }
  // I2C
  else
  {
    // Clear D/C to switch to command mode
    buff[0] = SSD_Command_Mode ;

    // Write Data on I2C
    fastI2Cwrite(buff, 3) ;
  }
}

void ArduiPi_OLED::sendCommand(uint8_t c0, uint8_t c1, uint8_t c2) 
{ 
  char buff[4] ;
    
  buff[1] = c0;
  buff[2] = c1;
  buff[3] = c2;

  // Is SPI
  if (isSPI())
  {
    // Setup D/C line to low to switch to command mode
    bcm2835_gpio_write(dc, LOW);

    // Write Data
    fastSPIwrite(&buff[1], 3);
  }
  // I2C
  else
  {
    // Clear D/C to switch to command mode
    buff[0] = SSD_Command_Mode; 

    // Write Data on I2C
    fastI2Cwrite(buff, sizeof(buff))  ;
  }
}


// Hardware scrolling.
//
// The SSD1306 family scrolls in hardware, and so does the CH1116, found
// on boards sold as SH1106 (see begin()), with its own command layout. The SH1106 has no scroll commands: sent to it, a scroll's
// bytes are read as other commands, so on it (and on the SSD1327 96x96)
// no scroll is started. But some boards sold as SH1106 carry a clone
// controller (CH1116 and the like) that reads back its RAM as the SH1106
// does and also has the SSD1306's hardware scroll: a scroll started there
// keeps running through restarts until power is cut. So stopscroll()
// always sends "deactivate scroll" (0x2E), on every type, as this library
// always did: a genuine SH1106 ignores it.
//
// The SSD1306 datasheet forbids writing its RAM, or changing the scroll
// settings, while a scroll is running, and says the RAM must be rewritten
// after a scroll stops (a horizontal scroll moves the RAM contents). So a
// scroll is stopped before its settings change, and display() stops the
// running scroll, writes the frame, and starts it again.
//
// Pages are 8 rows: 0-7 on a 64-row display, 0-3 on a 32-row one. Callers
// pass 0x0F for "to the bottom" (written for a 16-page display): pages are
// clamped to the display's own.

boolean ArduiPi_OLED::isSSD1306Family(void)
{
  return oled_type == OLED_ADAFRUIT_SPI_128x32 || oled_type == OLED_ADAFRUIT_SPI_128x64 ||
         oled_type == OLED_ADAFRUIT_I2C_128x32 || oled_type == OLED_ADAFRUIT_I2C_128x64 ||
         oled_type == OLED_SEEED_I2C_128x64;
}

boolean ArduiPi_OLED::hasHWScroll(void)
{
  return isSSD1306Family() || ch1116;
}

// The vertical and horizontal (diagonal) scroll is the SSD1306's own; the
// SSD1308 (Seeed 128x64) and the CH1116 scroll only horizontally.
boolean ArduiPi_OLED::hasDiagScroll(void)
{
  return isSSD1306Family() && oled_type != OLED_SEEED_I2C_128x64;
}

uint8_t ArduiPi_OLED::scrollPage(uint8_t page)
{
  uint8_t last = (oled_height / 8) - 1;
  return page > last ? last : page;
}

void ArduiPi_OLED::scrollClear(void)
{
  scroll_len  = 0;
  scroll_ncmd = 0;
}

void ArduiPi_OLED::scrollAdd(const uint8_t *c, uint8_t len)
{
  if (scroll_ncmd >= sizeof(scroll_cmdlen) || scroll_len + len > sizeof(scroll_seq))
    return;
  memcpy(scroll_seq + scroll_len, c, len);
  scroll_len += len;
  scroll_cmdlen[scroll_ncmd++] = len;
}

// A scroll's setup is sent a command at a time, each command with its
// parameters in one I2C transaction of its own, then the activation.
// Some SSD1306-compatible controllers take a command only that way: sent
// byte by byte (one transaction per byte), or run together with the next
// command in one transaction, its parameters were lost.
void ArduiPi_OLED::sendScroll(void)
{
  uint8_t off = 0;
  for (uint8_t i = 0; i < scroll_ncmd; i++)
  {
    sendCommands(scroll_seq + off, scroll_cmdlen[i]);
    off += scroll_cmdlen[i];
  }
  sendCommand(SSD_Activate_Scroll);
}

// A horizontal scroll of pages start through stop (cmd: 26h right, 27h
// left).
//
// The CH1116 (datasheet, 24h and 26h/27h): the window of columns that
// scroll, set to the 128 visible ones of its 132 (2-129), as the 4
// off-screen columns hold whatever was there at power-up and would sweep
// across the panel at the end of each pass; then start page, time
// interval (00h: 6 frames, near the SSD1306's 5) and end page, with no
// dummy bytes. Writing its RAM while it scrolls is allowed.
//
// The SSD1306: a diagonal scroll (the standby screen's) leaves its
// vertical offset and vertical scroll area set, and the panel then rolls
// the whole screen up during a later horizontal scroll. So the offset is
// cleared (a vertical-and-horizontal setup with offset 0, not started)
// and the area set to no rows (A3h 00h 00h), before the horizontal setup:
// dummy, start page, interval (5 frames), end page, dummy 00h, FFh.
void ArduiPi_OLED::startHorizontal(uint8_t cmd, uint8_t start, uint8_t stop)
{
  if (!hasHWScroll())
    return;
  stopscroll();
  start = scrollPage(start);
  stop  = scrollPage(stop);
  if (stop < start)
    stop = start;
  scrollClear();
  if (ch1116)
  {
    const uint8_t window[] = { 0x24, 0x02, 0x81 };
    const uint8_t setup[]  = { cmd, start, 0x00, stop };
    scrollAdd(window, sizeof(window));
    scrollAdd(setup, sizeof(setup));
  }
  else
  {
    if (hasDiagScroll()) // the vertical scroll: SSD1306 only (not the SSD1308)
    {
      const uint8_t offset[] = { SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL, 0x00, 0x00, 0x00, 0x00, 0x00 };
      const uint8_t area[]   = { SSD1306_SET_VERTICAL_SCROLL_AREA, 0x00, 0x00 };
      scrollAdd(offset, sizeof(offset));
      scrollAdd(area, sizeof(area));
    }
    const uint8_t setup[] = { cmd, 0x00, start, 0x00, stop, 0x00, 0xFF };
    scrollAdd(setup, sizeof(setup));
  }
  sendScroll();
  scrolling = true;
}

// A diagonal scroll (SSD1306 family; cmd: 29h right, 2Ah left): the
// vertical scroll area, no fixed rows and all rows scrolling; then the
// setup. The classic SSD1306 takes dummy, start page, interval, end page,
// vertical offset. The SSD1315 takes 7 bytes: A[0] horizontal scroll on
// (1) or off (0), start page, interval, end page, vertical offset, start
// column, end column (SSD1315 datasheet; it drops the command when they
// are not all there, and with A=0 rolls the screen vertically only). So:
// A=01h (a dummy on the SSD1306), and start column 00h, end column FFh
// (the SSD1315 takes 7 bits, 127; on the SSD1306, set column 0 and not a
// command). The CH1116 has none: it scrolls the same pages across.
void ArduiPi_OLED::startDiagonal(uint8_t cmd, uint8_t start, uint8_t stop)
{
  if (ch1116)
  {
    startHorizontal(cmd == SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL ? SSD_Right_Horizontal_Scroll : SSD_Left_Horizontal_Scroll, start, stop);
    return;
  }
  if (!hasDiagScroll())
    return;
  stopscroll();
  start = scrollPage(start);
  stop  = scrollPage(stop);
  if (stop < start)
    stop = start;
  scrollClear();
  const uint8_t area[]  = { SSD1306_SET_VERTICAL_SCROLL_AREA, 0x00, (uint8_t)oled_height };
  const uint8_t setup[] = { cmd, 0x01, start, 0x00, stop, 0x01, 0x00, 0xFF };
  scrollAdd(area, sizeof(area));
  scrollAdd(setup, sizeof(setup));
  sendScroll();
  scrolling = true;
}

void ArduiPi_OLED::startscrollright(uint8_t start, uint8_t stop)
{
  startHorizontal(SSD_Right_Horizontal_Scroll, start, stop);
}

void ArduiPi_OLED::startscrollleft(uint8_t start, uint8_t stop)
{
  startHorizontal(SSD_Left_Horizontal_Scroll, start, stop);
}

void ArduiPi_OLED::startscrolldiagright(uint8_t start, uint8_t stop)
{
  startDiagonal(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL, start, stop);
}

void ArduiPi_OLED::startscrolldiagleft(uint8_t start, uint8_t stop)
{
  startDiagonal(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL, start, stop);
}


void ArduiPi_OLED::setHorizontalScrollProperties(bool direction,uint8_t startRow, uint8_t endRow,uint8_t startColumn, uint8_t endColumn, uint8_t scrollSpeed)
{
  // SSD1306 layout only
  if (!isSSD1306Family())
    return;
  // Settings must not change while a scroll runs
  stopscroll();
  if(Scroll_Right == direction)
  {
      //Scroll Right
      sendCommand(SSD_Left_Horizontal_Scroll);
  }
  else
  {
      //Scroll Left  
      sendCommand(SSD_Right_Horizontal_Scroll);
  }
  sendCommand(0x00);       //Dummmy byte
  sendCommand(scrollPage(startRow));
  sendCommand(scrollSpeed);
  sendCommand(scrollPage(endRow));
  sendCommand(startColumn+8);
  sendCommand(endColumn+8);
  sendCommand(0x00);      //Dummmy byte

}

void ArduiPi_OLED::stopscroll(void)
{
  // On every type: see above (SH1106 clones scroll too)
  scrolling = false;
  sendCommand(SSD_Deactivate_Scroll);
}

void ArduiPi_OLED::sendData(uint8_t c) 
{
  // SPI
  if ( isSPI())
  {
    // SPI
    // Setup D/C line to high to switch to data mode
    bcm2835_gpio_write(dc, HIGH);

    // write value
    fastSPIwrite(c);
  }
  // I2C
  else
  {
    char buff[2] ;
    
    // Setup D/C to switch to data mode
    buff[0] = SSD_Data_Mode; 
    buff[1] = c;

    // Write on i2c
    fastI2Cwrite( buff, sizeof(buff)) ;
  }
}

void ArduiPi_OLED::display(void) 
{

  if (oled_type == OLED_SEEED_I2C_96x96 )
  {
    sendCommand(SSD1327_Set_Row_Address   , 0x00, 0x5F);
    sendCommand(SSD1327_Set_Column_Address, 0x08, 0x37);
  }
  else
  {
    sendCommand(SSD1306_Set_Lower_Column_Start_Address  | 0x0); // low col = 0
    sendCommand(SSD1306_Set_Higher_Column_Start_Address | 0x0); // hi col = 0
    sendCommand(SSD1306_Set_Start_Line  | 0x0); // line #0
  }

  // No RAM writes while the SSD1306 scrolls: stop it for the frame, and
  // start it again after (the frame then rewrites the RAM the scroll moved)
  // (The CH1116 allows RAM writes while it scrolls: its scroll keeps going)
  boolean resume = scrolling && !ch1116;
  if (resume)
    sendCommand(SSD_Deactivate_Scroll);

  // The SSD1306 family runs in horizontal addressing mode (begin()), where
  // the column commands above do nothing: without its window set again,
  // a frame starts wherever the last one stopped, and a single byte lost
  // on the bus shifts every frame after it, the picture creeping across
  // the screen. Set the window (as begin() does) for every frame.
  if (isSSD1306Family())
  {
    sendCommand(SSD_Set_Column_Address, 0, 127);
    sendCommand(SSD_Set_Page_Address, 0, 7);
  }

  uint16_t i=0 ;
  
  // pointer to OLED data buffer
  uint8_t * p = poledbuff;

  // SPI
  if ( isSPI())
  {
    // Setup D/C line to high to switch to data mode
    bcm2835_gpio_write(dc, HIGH);

    // Send all data to OLED
    for ( i=0; i<oled_buff_size; i++) 
    {
      fastSPIwrite(*p++);
    }

    // I wonder why we have to do this (check datasheet)
    if (oled_height == 32) 
    {
      for (uint16_t i=0; i<oled_buff_size; i++) 
      {
        fastSPIwrite(0);
      }
    }
    
  }
  // I2C
  else 
  {
    char buff[17] ;
    uint8_t x ;

    // Setup D/C to switch to data mode
    buff[0] = SSD_Data_Mode; 
    
    if (oled_type == OLED_SH1106_I2C_128x64)
    {
      for (uint8_t k=0; k<8; k++) 
      {
        sendCommand(0xB0+k);//set page addressSSD_Data_Mode;
        sendCommand(0x02) ;//set lower column address
        sendCommand(0x10) ;//set higher column address

       for( i=0; i<8; i++)
       {
          for (x=1; x<=16; x++) 
            buff[x] = *p++;

          fastI2Cwrite(buff, 17);
        }
      }
    }
    else
    {
      // loop trough all OLED buffer and 
      // send a bunch of 16 data byte in one xmission
      for ( i=0; i<oled_buff_size; i+=16 ) 
      {
        for (x=1; x<=16; x++) 
          buff[x] = *p++;

        fastI2Cwrite(buff, 17);
      }
    }
  }

  if (resume)
    sendScroll();
}

// clear everything (in the buffer)
void ArduiPi_OLED::clearDisplay(void) 
{
  memset(poledbuff, 0, oled_buff_size);
}



