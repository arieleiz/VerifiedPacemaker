/* mbed TextLCD Library, for a 4-bit LCD based on HD44780
 * Copyright (c) 2007-2010, sford, http://mbed.org
 *               2013, v01: WH, Added LCD types, fixed LCD address issues, added Cursor and UDCs 
 *               2013, v02: WH, Added I2C and SPI bus interfaces
 *               2013, v03: WH, Added support for LCD40x4 which uses 2 controllers   
 *               2013, v04: WH, Added support for Display On/Off, improved 4bit bootprocess  
 *               2013, v05: WH, Added support for 8x2B, added some UDCs  
 *               2013, v06: WH, Added support for devices that use internal DC/DC converters 
 *               2013, v07: WH, Added support for backlight and include portdefinitions for LCD2004 Module from DFROBOT
 *               2014, v08: WH, Refactored in Base and Derived Classes to deal with mbed lib change regarding 'NC' defined DigitalOut pins
 *               2014, v09: WH/EO, Added Class for Native SPI controllers such as ST7032 
 *               2014, v10: WH, Added Class for Native I2C controllers such as ST7032i, Added support for MCP23008 I2C portexpander, Added support for Adafruit module  
 *               2014, v11: WH, Added support for native I2C controllers such as PCF21XX, Improved the _initCtrl() method to deal with differences between all supported controllers  
 *               2014, v12: WH, Added support for native I2C controller PCF2119 and native I2C/SPI controllers SSD1803, ST7036, added setContrast method (by JH1PJL) for supported devices (eg ST7032i) 
 *               2014, v13: WH, Added support for controllers US2066/SSD1311 (OLED), added setUDCBlink method for supported devices (eg SSD1803), fixed issue in setPower() 
 *@Todo Add AC780S/KS0066i
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MBED_TEXTLCD_H
#define MBED_TEXTLCD_H

#include "mbed.h"
#include "simhw.hpp"
#include <assert.h>

/** A TextLCD interface for driving 4-bit HD44780-based LCDs
 *
 * Currently supports 8x1, 8x2, 12x3, 12x4, 16x1, 16x2, 16x3, 16x4, 20x2, 20x4, 24x1, 24x2, 24x4, 40x2 and 40x4 panels
 * Interface options include direct mbed pins, I2C portexpander (PCF8474/PCF8574A or MCP23008) or SPI bus shiftregister (74595). 
 * Supports some controllers with native I2C or SP interface. Supports some controllers that provide internal DC/DC converters for VLCD or VLED. 
 *
 * @code
 * #include "mbed.h"
 * #include "TextLCD.h"
 * 
 * // I2C Communication
 * I2C i2c_lcd(p28,p27); // SDA, SCL
 *
 * // SPI Communication
 * SPI spi_lcd(p5, NC, p7); // MOSI, MISO, SCLK
 *
 * //TextLCD lcd(p15, p16, p17, p18, p19, p20);                          // RS, E, D4-D7, LCDType=LCD16x2, BL=NC, E2=NC, LCDTCtrl=HD44780
 * //TextLCD_SPI lcd(&spi_lcd, p8, TextLCD::LCD40x4);                    // SPI bus, 74595 expander, CS pin, LCD Type  
 * TextLCD_I2C lcd(&i2c_lcd, 0x42, TextLCD::LCD20x4);                  // I2C bus, PCF8574 Slaveaddress, LCD Type
 * //TextLCD_I2C lcd(&i2c_lcd, 0x42, TextLCD::LCD16x2, TextLCD::WS0010); // I2C bus, PCF8574 Slaveaddress, LCD Type, Device Type
 * //TextLCD_SPI_N lcd(&spi_lcd, p8, p9);                                // SPI bus, CS pin, RS pin, LCDType=LCD16x2, BL=NC, LCDTCtrl=ST7032_3V3   
 * //TextLCD_I2C_N lcd(&i2c_lcd, ST7032_SA, TextLCD::LCD16x2, NC, TextLCD::ST7032_3V3); // I2C bus, Slaveaddress, LCD Type, BL=NC, LCDTCtrl=ST7032_3V3  
 * 
 * int main() {
 *   lcd.printf("Hello World!\n");
 * }
 * @endcode
 */


//Pin Defines for I2C PCF8574/PCF8574A or MCP23008 and SPI 74595 bus expander interfaces
//LCD and serial portexpanders should be wired accordingly 
//
//Select Hardware module (one option only)
#define DEFAULT        1
#define ADAFRUIT       0
#define DFROBOT        0

#if (DEFAULT==1)
//Definitions for default (WH) mapping between serial port expander pins and LCD controller
//This hardware supports the I2C bus expander (PCF8574/PCF8574A or MCP23008) and SPI bus expander (74595) interfaces
//See https://mbed.org/cookbook/Text-LCD-Enhanced
//
//Note: LCD RW pin must be connected to GND
//      E2 is used for LCD40x4 (second controller)
//      BL may be used to control backlight
#define D_LCD_PIN_D4   0
#define D_LCD_PIN_D5   1
#define D_LCD_PIN_D6   2
#define D_LCD_PIN_D7   3
#define D_LCD_PIN_RS   4
#define D_LCD_PIN_E    5
#define D_LCD_PIN_E2   6
#define D_LCD_PIN_BL   7

#define D_LCD_PIN_RW   D_LCD_PIN_E2

//Select I2C Portexpander type (one option only)
#define PCF8574        1
#define MCP23008       0
#endif

#if (ADAFRUIT==1)
//Definitions for Adafruit i2cspilcdbackpack mapping between serial port expander pins and LCD controller
//This hardware supports both an I2C expander (MCP23008) and an SPI expander (74595) selectable by a jumper.
//See http://www.ladyada.net/products/i2cspilcdbackpack
//
//Note: LCD RW pin must be kept LOW
//      E2 is not available on this hardware and so it does not support LCD40x4 (second controller)
//      BL is used to control backlight
#define D_LCD_PIN_0    0
#define D_LCD_PIN_RS   1
#define D_LCD_PIN_E    2
#define D_LCD_PIN_D4   3
#define D_LCD_PIN_D5   4
#define D_LCD_PIN_D6   5
#define D_LCD_PIN_D7   6
#define D_LCD_PIN_BL   7

#define D_LCD_PIN_E2   D_LCD_PIN_0

//Force I2C portexpander type
#define PCF8574        0
#define MCP23008       1
#endif

//Bitpattern Defines for I2C PCF8574/PCF8574A, MCP23008 and SPI 74595 Bus expanders
//
#define D_LCD_D4       (1<<D_LCD_PIN_D4)
#define D_LCD_D5       (1<<D_LCD_PIN_D5)
#define D_LCD_D6       (1<<D_LCD_PIN_D6)
#define D_LCD_D7       (1<<D_LCD_PIN_D7)
#define D_LCD_RS       (1<<D_LCD_PIN_RS)
#define D_LCD_E        (1<<D_LCD_PIN_E)
#define D_LCD_E2       (1<<D_LCD_PIN_E2)
#define D_LCD_BL       (1<<D_LCD_PIN_BL)
//#define D_LCD_RW       (1<<D_LCD_PIN_RW)

#define D_LCD_BUS_MSK  (D_LCD_D4 | D_LCD_D5 | D_LCD_D6 | D_LCD_D7)
#define D_LCD_BUS_DEF  0x00

/* PCF8574/PCF8574A I2C portexpander slave address */
#define PCF8574_SA0    0x40
#define PCF8574_SA1    0x42
#define PCF8574_SA2    0x44
#define PCF8574_SA3    0x46
#define PCF8574_SA4    0x48
#define PCF8574_SA5    0x4A
#define PCF8574_SA6    0x4C
#define PCF8574_SA7    0x4E

#define PCF8574A_SA0   0x70
#define PCF8574A_SA1   0x72
#define PCF8574A_SA2   0x74
#define PCF8574A_SA3   0x76
#define PCF8574A_SA4   0x78
#define PCF8574A_SA5   0x7A
#define PCF8574A_SA6   0x7C
#define PCF8574A_SA7   0x7E

/* MCP23008 I2C portexpander slave address */
#define MCP23008_SA0   0x40
#define MCP23008_SA1   0x42
#define MCP23008_SA2   0x44
#define MCP23008_SA3   0x46
#define MCP23008_SA4   0x48
#define MCP23008_SA5   0x4A
#define MCP23008_SA6   0x4C
#define MCP23008_SA7   0x4E

/* MCP23008 I2C portexpander internal registers */
#define IODIR          0x00
#define IPOL           0x01
#define GPINTEN        0x02
#define DEFVAL         0x03
#define INTCON         0x04
#define IOCON          0x05
#define GPPU           0x06
#define INTF           0x07
#define INTCAP         0x08
#define GPIO           0x09
#define OLAT           0x0A


/* ST7032i I2C slave address */
#define ST7032_SA      0x7C

/* ST7036i I2C slave address */
#define ST7036_SA0     0x78
#define ST7036_SA1     0x7A
#define ST7036_SA2     0x7C
#define ST7036_SA3     0x7E

/* PCF21XX I2C slave address */
#define PCF21XX_SA0    0x74
#define PCF21XX_SA1    0x76

/* AIP31068 I2C slave address */
#define AIP31068_SA    0x7C

/* SSD1803 I2C slave address */
#define SSD1803_SA0    0x78
#define SSD1803_SA1    0x7A

/* US2066/SSD1311 I2C slave address */
#define US2066_SA0     0x78
#define US2066_SA1     0x7A

/* AC780 I2C slave address */
#define AC780_SA0      0x78
#define AC780_SA1      0x7A
#define AC780_SA2      0x7C
#define AC780_SA3      0x7E

//Some native I2C controllers dont support ACK. Set define to '0' to allow code to proceed even without ACK
//#define LCD_I2C_ACK    0
#define LCD_I2C_ACK    1

/* LCD Type information on Rows, Columns and Variant. This information is encoded in
 * an int and used for the LCDType enumerators in order to simplify code maintenance */
// Columns encoded in b7..b0
#define LCD_T_COL_MSK  0x000000FF
#define LCD_T_C6       0x00000006
#define LCD_T_C8       0x00000008
#define LCD_T_C10      0x0000000A
#define LCD_T_C12      0x0000000C
#define LCD_T_C16      0x00000010
#define LCD_T_C20      0x00000014
#define LCD_T_C24      0x00000018
#define LCD_T_C32      0x00000020
#define LCD_T_C40      0x00000028

// Rows encoded in b15..b8  
#define LCD_T_ROW_MSK  0x0000FF00
#define LCD_T_R1       0x00000100
#define LCD_T_R2       0x00000200
#define LCD_T_R3       0x00000300
#define LCD_T_R4       0x00000400
  
// Addressing mode encoded in b19..b16
#define LCD_T_ADR_MSK  0x000F0000
#define LCD_T_A        0x00000000  /*Mode A   Default 1, 2 or 4 line display                       */
#define LCD_T_B        0x00010000  /*Mode B,  Alternate 8x2 (actually 16x1 display)                */
#define LCD_T_C        0x00020000  /*Mode C,  Alternate 16x1 (actually 8x2 display)                */
#define LCD_T_D        0x00030000  /*Mode D,  Alternate 3 or 4 line display (12x4, 20x4, 24x4)     */
#define LCD_T_D1       0x00040000  /*Mode D1, Alternate 3 out of 4 line display (12x3, 20x3, 24x3) */
#define LCD_T_E        0x00050000  /*Mode E,  40x4 display (actually two 40x2)                     */
#define LCD_T_F        0x00060000  /*Mode F,  16x3 display (actually 24x2)                         */
#define LCD_T_G        0x00070000  /*Mode G,  16x3 display                                         */

/* LCD Ctrl information on interface support and features. This information is encoded in
 * an int and used for the LCDCtrl enumerators in order to simplify code maintenance */
// Interface encoded in b31..b24
#define LCD_C_BUS_MSK  0xFF000000
#define LCD_C_PAR      0x01000000  /*Parallel 4 or 8 bit data, E pin, RS pin, RW=GND            */
#define LCD_C_SPI3_9   0x02000000  /*SPI 3 line (MOSI, SCL, CS pins),  9 bits (RS + 8 Data)     */
#define LCD_C_SPI3_10  0x04000000  /*SPI 3 line (MOSI, SCL, CS pins), 10 bits (RS, RW + 8 Data) */
#define LCD_C_SPI3_16  0x08000000  /*SPI 3 line (MOSI, SCL, CS pins), 16 bits (RS, RW + 8 Data) */
#define LCD_C_SPI3_24  0x10000000  /*SPI 3 line (MOSI, SCL, CS pins), 24 bits (RS, RW + 8 Data) */
#define LCD_C_SPI4     0x20000000  /*SPI 4 line (MOSI, SCL, CS, RS pin), RS pin + 8 Data        */
#define LCD_C_I2C      0x40000000  /*I2C (SDA, SCL pin), 8 control bits (Co, RS, RW) + 8 Data   */
// Features encoded in b23..b16
#define LCD_C_FTR_MSK  0x00FF0000 
#define LCD_C_BST      0x00010000  /*Booster             */
#define LCD_C_CTR      0x00020000  /*Contrast Control    */
#define LCD_C_ICN      0x00040000  /*Icons               */
#define LCD_C_PDN      0x00080000  /*Power Down          */


// Contrast setting, 6 significant bits (only supported for controllers with extended features)
// Voltage Multiplier setting, 2 or 3 significant bits (only supported for controllers with extended features)
#define LCD_DEF_CONTRAST    0x20

//ST7032 EastRising ERC1602FS-4 display
//Contrast setting 6 significant bits
//Voltage Multiplier setting 3 significant bits
#define LCD_ST7032_CONTRAST 0x18
#define LCD_ST7032_RAB      0x04

//ST7036 EA DOGM1603 display
//Contrast setting 6 significant bits
//Voltage Multiplier setting 3 significant bits
#define LCD_ST7036_CONTRAST 0x28
#define LCD_ST7036_RAB      0x04

//SSD1803 EA DOGM204 display
//Contrast setting 6 significant bits
//Voltage Multiplier setting 3 significant bits
#define LCD_SSD1_CONTRAST   0x28
#define LCD_SSD1_RAB        0x06

//US2066/SSD1311 EastRising ER-OLEDM2002-4 display
//Contrast setting 8 significant bits, use 6 for compatibility
#define LCD_US20_CONTRAST   0x3F
//#define LCD_US20_CONTRAST   0x1F

//PCF2113, PCF2119 display
//Contrast setting 6 significant bits
//Voltage Multiplier setting 2 significant bits
#define LCD_PCF2_CONTRAST   0x20
#define LCD_PCF2_S12        0x02

//PT6314 VFD display
//Contrast setting 2 significant bits
#define LCD_PT63_CONTRAST   0x00

/** Some sample User Defined Chars 5x7 dots */
//extern const char udc_ae[];      //æ
//extern const char udc_0e[];      //ø
//extern const char udc_ao[];      //å
//extern const char udc_AE[];      //Æ
//extern const char udc_0E[];      //Ø
//extern const char udc_Ao[];      //Å
//extern const char udc_PO[];      //Padlock Open
//extern const char udc_PC[];      //Padlock Closed

//extern const char udc_alpha[];  //alpha
//extern const char udc_ohm[];    //ohm
//extern const char udc_sigma[];  //sigma
//extern const char udc_pi[];     //pi
//extern const char udc_root[];   //root


extern const char udc_0[];       // |>
extern const char udc_1[];       // <|
extern const char udc_2[];       // |
extern const char udc_3[];       // ||
extern const char udc_4[];       // |||
extern const char udc_5[];       // =
extern const char udc_6[];       // checkerboard
extern const char udc_7[];       // \

extern const char udc_degr[];    // Degree symbol

extern const char udc_TM_T[];    // Trademark T
extern const char udc_TM_M[];    // Trademark M

//extern const char udc_Bat_Hi[];  // Battery Full
//extern const char udc_Bat_Ha[];  // Battery Half
//extern const char udc_Bat_Lo[];  // Battery Low
extern const char udc_Bat_Hi[];  // Battery Full
extern const char udc_Bat_Ha[];  // Battery Half
extern const char udc_Bat_Lo[];  // Battery Low
extern const char udc_AC[];      // AC Power

//extern const char udc_smiley[];  // Smiley
//extern const char udc_droopy[];  // Droopey
//extern const char udc_note[];    // Note

//extern const char udc_bar_1[];   // Bar 1
//extern const char udc_bar_2[];   // Bar 11
//extern const char udc_bar_3[];   // Bar 111
//extern const char udc_bar_4[];   // Bar 1111
//extern const char udc_bar_5[];   // Bar 11111

//extern const char udc_ch_1[];    // Hor bars 4
//extern const char udc_ch_2[];    // Hor bars 4 (inverted)
//extern const char udc_ch_3[];    // Ver bars 3
//extern const char udc_ch_4[];    // Ver bars 3 (inverted)
//extern const char udc_ch_yr[];   // Year   (kana)
//extern const char udc_ch_mo[];   // Month  (kana)
//extern const char udc_ch_dy[];   // Day    (kana)
//extern const char udc_ch_mi[];   // minute (kana)
extern const char udc_None[]; 
extern const char udc_All[];

/** A TextLCD interface for driving 4-bit HD44780-based LCDs
 *
 * @brief Currently supports 8x1, 8x2, 12x2, 12x3, 12x4, 16x1, 16x2, 16x3, 16x4, 20x2, 20x4, 24x2, 24x4, 40x2 and 40x4 panels
 *        Interface options include direct mbed pins, I2C portexpander (PCF8474/PCF8574A or MCP23008) or SPI bus shiftregister (74595) and some native I2C or SPI devices 
 *
 */
class TextLCD_Base : public Stream {
public:

    /** LCD panel format */
    // The commented out types exist but have not yet been tested with the library
    enum LCDType {
//        LCD6x1     = (LCD_T_A | LCD_T_C6 | LCD_T_R1),     /**<  6x1 LCD panel */          
//        LCD6x2     = (LCD_T_A | LCD_T_C6 | LCD_T_R2),     /**<  6x2 LCD panel */          
        LCD8x1     = (LCD_T_A | LCD_T_C8 | LCD_T_R1),     /**<  8x1 LCD panel */    
        LCD8x2     = (LCD_T_A | LCD_T_C8 | LCD_T_R2),     /**<  8x2 LCD panel */          
        LCD8x2B    = (LCD_T_D | LCD_T_C8 | LCD_T_R2),     /**<  8x2 LCD panel (actually 16x1) */                  
//        LCD12x1    = (LCD_T_A | LCD_T_C12 | LCD_T_R1),    /**< 12x1 LCD panel */                          
        LCD12x2    = (LCD_T_A | LCD_T_C12 | LCD_T_R2),    /**< 12x2 LCD panel */                          
        LCD12x3D   = (LCD_T_D | LCD_T_C12 | LCD_T_R3),    /**< 12x3 LCD panel, special mode PCF21XX */                                  
        LCD12x3D1  = (LCD_T_D1 | LCD_T_C12 | LCD_T_R3),   /**< 12x3 LCD panel, special mode PCF21XX */                                          
//        LCD12x3G   = (LCD_T_G | LCD_T_C12 | LCD_T_R3),    /**< 12x3 LCD panel, special mode ST7036 */                                      
        LCD12x4    = (LCD_T_A | LCD_T_C12 | LCD_T_R4),    /**< 12x4 LCD panel */                  
        LCD12x4D   = (LCD_T_B | LCD_T_C12 | LCD_T_R4),    /**< 12x4 LCD panel, special mode PCF21XX */                                          
        LCD16x1    = (LCD_T_A | LCD_T_C16 | LCD_T_R1),    /**< 16x1 LCD panel */                  
        LCD16x1C   = (LCD_T_C | LCD_T_C16 | LCD_T_R1),    /**< 16x1 LCD panel (actually 8x2) */          
        LCD16x2    = (LCD_T_A | LCD_T_C16 | LCD_T_R2),    /**< 16x2 LCD panel (default) */
//        LCD16x2B   = (LCD_T_B | LCD_T_C16 | LCD_T_R2),    /**< 16x2 LCD panel, alternate addressing, wrong.. */
        LCD16x3D   = (LCD_T_D | LCD_T_C16 | LCD_T_R3),    /**< 16x3 LCD panel, special mode SSD1803 */                      
//        LCD16x3D1  = (LCD_T_D1 | LCD_T_C16 | LCD_T_R3),   /**< 16x3 LCD panel, special mode SSD1803 */                
        LCD16x3F   = (LCD_T_F | LCD_T_C16 | LCD_T_R3),    /**< 16x3 LCD panel (actually 24x2) */                
        LCD16x3G   = (LCD_T_G | LCD_T_C16 | LCD_T_R3),    /**< 16x3 LCD panel, special mode ST7036 */                              
        LCD16x4    = (LCD_T_A | LCD_T_C16 | LCD_T_R4),    /**< 16x4 LCD panel */        
//        LCD16x4D   = (LCD_T_D | LCD_T_C16 | LCD_T_R4),    /**< 16x4 LCD panel, special mode SSD1803 */                
//        LCD20x1    = (LCD_T_A | LCD_T_C20 | LCD_T_R1),    /**< 20x1 LCD panel */
        LCD20x2    = (LCD_T_A | LCD_T_C20 | LCD_T_R2),    /**< 20x2 LCD panel */
//        LCD20x3    = (LCD_T_A | LCD_T_C20 | LCD_T_R3),    /**< 20x3 LCD panel */                        
//        LCD20x3D   = (LCD_T_D | LCD_T_C20 | LCD_T_R3),    /**< 20x3 LCD panel, special mode SSD1803 */                        
//        LCD20x3D1  = (LCD_T_D1 | LCD_T_C20 | LCD_T_R3),   /**< 20x3 LCD panel, special mode SSD1803 */                        
        LCD20x4    = (LCD_T_A | LCD_T_C20 | LCD_T_R4),    /**< 20x4 LCD panel */
        LCD20x4D   = (LCD_T_D | LCD_T_C20 | LCD_T_R4),    /**< 20x4 LCD panel, special mode SSD1803 */                        
        LCD24x1    = (LCD_T_A | LCD_T_C24 | LCD_T_R1),    /**< 24x1 LCD panel */        
        LCD24x2    = (LCD_T_A | LCD_T_C24 | LCD_T_R2),    /**< 24x2 LCD panel */        
//        LCD24x3D   = (LCD_T_D | LCD_T_C24 | LCD_T_R3),    /**< 24x3 LCD panel */                
//        LCD24x3D1  = (LCD_T_D | LCD_T_C24 | LCD_T_R3),    /**< 24x3 LCD panel */                        
        LCD24x4D   = (LCD_T_D | LCD_T_C24 | LCD_T_R4),    /**< 24x4 LCD panel, special mode KS0078 */                                                          
//        LCD32x1    = (LCD_T_A | LCD_T_C32 | LCD_T_R1),    /**< 32x1 LCD panel */
//        LCD32x1C   = (LCD_T_C | LCD_T_C32 | LCD_T_R1),    /**< 32x1 LCD panel (actually 16x2) */                                        
//        LCD32x2    = (LCD_T_A | LCD_T_C32 | LCD_T_R2),    /**< 32x2 LCD panel */                        
//        LCD32x4    = (LCD_T_A | LCD_T_C32 | LCD_T_R4),    /**< 32x4 LCD panel */                        
//        LCD40x1    = (LCD_T_A | LCD_T_C40 | LCD_T_R1),    /**< 40x1 LCD panel */                        
//        LCD40x1C   = (LCD_T_C | LCD_T_C40 | LCD_T_R1),    /**< 40x1 LCD panel (actually 20x2) */                        
        LCD40x2    = (LCD_T_A | LCD_T_C40 | LCD_T_R2),    /**< 40x2 LCD panel */                
        LCD40x4    = (LCD_T_E | LCD_T_C40 | LCD_T_R4)     /**< 40x4 LCD panel, Two controller version */                        
    };


    /** LCD Controller Device */
    enum LCDCtrl {
        HD44780     = 0 |  LCD_C_PAR,                                                                    /**<  HD44780 or full equivalent (default)         */    
        WS0010      = 1 | (LCD_C_PAR | LCD_C_SPI3_10 | LCD_C_BST),                                       /**<  WS0010  OLED Controller, 4/8 bit, SPI3       */    
        ST7036_3V3  = 2 | (LCD_C_PAR | LCD_C_SPI4    | LCD_C_I2C | LCD_C_BST | LCD_C_CTR),               /**<  ST7036  3V3 with Booster, 4/8 bit, SPI4, I2C */   
        ST7036_5V   = 3 | (LCD_C_PAR | LCD_C_SPI4    | LCD_C_I2C | LCD_C_BST | LCD_C_CTR),               /**<  ST7036  5V no Booster, 4/8 bit, SPI4, I2C    */   
        ST7032_3V3  = 4 | (LCD_C_PAR | LCD_C_SPI4    | LCD_C_I2C | LCD_C_BST | LCD_C_CTR),               /**<  ST7032  3V3 with Booster, 4/8 bit, SPI4, I2C */   
        ST7032_5V   = 5 | (LCD_C_PAR | LCD_C_SPI4    | LCD_C_I2C | LCD_C_CTR),                           /**<  ST7032  5V no Booster, 4/8 bit, SPI4, I2C    */           
        KS0078      = 6 | (LCD_C_PAR | LCD_C_SPI3_24),                                                   /**<  KS0078  24x4 support, 4/8 bit, SPI3          */                   
        PCF2113_3V3 = 7 | (LCD_C_PAR | LCD_C_I2C     | LCD_C_BST | LCD_C_CTR),                           /**<  PCF2113 3V3 with Booster, 4/8 bit, I2C       */                           
        PCF2116_3V3 = 8 | (LCD_C_PAR | LCD_C_I2C     | LCD_C_BST),                                       /**<  PCF2116 3V3 with Booster, 4/8 bit, I2C       */                           
        PCF2116_5V  = 9 | (LCD_C_PAR | LCD_C_I2C),                                                       /**<  PCF2116 5V no Booster, 4/8 bit, I2C          */        
        PCF2119_3V3 = 10 | (LCD_C_PAR | LCD_C_I2C     | LCD_C_BST | LCD_C_CTR),                          /**<  PCF2119 3V3 with Booster, 4/8 bit, I2C       */                           
//        PCF2119_5V  = 11 | (LCD_C_PAR | LCD_C_I2C),                                                      /**<  PCF2119 5V no Booster, 4/8 bit, I2C          */
        AIP31068    = 12 | (LCD_C_SPI3_9  | LCD_C_I2C | LCD_C_BST),                                      /**<  AIP31068 I2C, SPI3                           */                           
        SSD1803_3V3 = 13 | (LCD_C_PAR | LCD_C_SPI3_24 | LCD_C_I2C | LCD_C_BST | LCD_C_CTR | LCD_C_PDN),  /**<  SSD1803 3V3 with Booster, 4/8 bit, I2C, SPI3 */
//        SSD1803_5V  = 14 | (LCD_C_PAR | LCD_C_SPI3_24 | LCD_C_I2C | LCD_C_BST | LCD_C_CTR | LCD_C_PDN),  /**<  SSD1803 3V3 with Booster, 4/8 bit, I2C, SPI3 */
        US2066_3V3  = 15 | (LCD_C_PAR | LCD_C_SPI3_24 | LCD_C_I2C | LCD_C_CTR | LCD_C_PDN),              /**<  US2066/SSD1311 3V3, 4/8 bit, I2C, SPI3 */
//        PT6314      = 16 | (LCD_C_PAR | LCD_C_SPI3_16 | LCD_C_CTR | LCD_C_PDN),                          /**<  PT6314  VFD, 4/8 bit, SPI3                   */
//        AC780       = 17 | (LCD_C_PAR | LCD_C_SPI4 | LCD_C_I2C | LCD_C_PDN),                             /**<  AC780  4/8 bit, SPI, I2C                     */
//        KS0066      = 18 | (LCD_C_PAR | LCD_C_SPI4 | LCD_C_I2C | LCD_C_PDN)                              /**<  KS0066i == AC780  4/8 bit, SPI, I2C          */
    };


    /** LCD Cursor control */
    enum LCDCursor {
        CurOff_BlkOff = 0x00,  /**<  Cursor Off, Blinking Char Off */    
        CurOn_BlkOff  = 0x02,  /**<  Cursor On, Blinking Char Off */    
        CurOff_BlkOn  = 0x01,  /**<  Cursor Off, Blinking Char On */    
        CurOn_BlkOn   = 0x03   /**<  Cursor On, Blinking Char On */    
    };

    /** LCD Display control */
    enum LCDMode {
        DispOff = 0x00,  /**<  Display Off */    
        DispOn  = 0x04   /**<  Display On */            
    };

   /** LCD Backlight control */
    enum LCDBacklight {
        LightOff,        /**<  Backlight Off */    
        LightOn          /**<  Backlight On */            
    };

   /** LCD Blink control (UDC), supported for some Controllers */
    enum LCDBlink {
        BlinkOff,        /**<  Blink Off */    
        BlinkOn          /**<  Blink On  */            
    };

   /** LCD Orientation control, supported for some Controllers */
    enum LCDOrient {
        Top,             /**<  Top view */    
        Bottom           /**<  Upside down view */            
    };


    /** Locate cursor to a screen column and row
     *
     * @param column  The horizontal position from the left, indexed from 0
     * @param row     The vertical position from the top, indexed from 0
     */
	void locate(int column, int row)
	{
		AESIM::textlcd_locate(this, column, row);
	}

    /** Return the memoryaddress of screen column and row location
     *
     * @param column  The horizontal position from the left, indexed from 0
     * @param row     The vertical position from the top, indexed from 0
     * @param return  The memoryaddress of screen column and row location
     */
	int  getAddress(int column, int row)
	{
		UNREFERENCED_PARAMETER(column);
		UNREFERENCED_PARAMETER(row);
		assert(false);
		return 0;
	}
    
    /** Set the memoryaddress of screen column and row location
     *
     * @param column  The horizontal position from the left, indexed from 0
     * @param row     The vertical position from the top, indexed from 0
     */
	void setAddress(int column, int row)
	{
		UNREFERENCED_PARAMETER(column);
		UNREFERENCED_PARAMETER(row);
		assert(false);
	}

    /** Clear the screen and locate to 0,0
     */
	void cls()
	{
		AESIM::textlcd_cls(this);
	}

    /** Return the number of rows
     *
     * @param return  The number of rows
     */
	int rows() { return _rows; }
    
    /** Return the number of columns
     *
     * @param return  The number of columns
     */  
	int columns() { return _cols; }

    /** Set the Cursormode
     *
     * @param cursorMode  The Cursor mode (CurOff_BlkOff, CurOn_BlkOff, CurOff_BlkOn, CurOn_BlkOn)
     */
	void setCursor(LCDCursor cursorMode) { 
		UNREFERENCED_PARAMETER(cursorMode);
	}
    
    /** Set the Displaymode
     *
     * @param displayMode The Display mode (DispOff, DispOn)
     */
	void setMode(LCDMode displayMode)
	{
		UNREFERENCED_PARAMETER(displayMode);
	}

    /** Set the Backlight mode
     *
     *  @param backlightMode The Backlight mode (LightOff, LightOn)
     */
	void setBacklight(LCDBacklight backlightMode)
	{
		UNREFERENCED_PARAMETER(backlightMode);
	}

    /** Set User Defined Characters (UDC)
     *
     * @param unsigned char c   The Index of the UDC (0..7)
     * @param char *udc_data    The bitpatterns for the UDC (8 bytes of 5 significant bits)     
     */
	void setUDC(unsigned char c, char *udc_data)
	{
		assert(false);
		UNREFERENCED_PARAMETER(c);
		UNREFERENCED_PARAMETER(udc_data);
	}

    /** Set UDC Blink
     * setUDCBlink method is supported by some compatible devices (eg SSD1803) 
     *
     * @param blinkMode The Blink mode (BlinkOff, BlinkOn)
     */
	void setUDCBlink(LCDBlink blinkMode)
	{
		UNREFERENCED_PARAMETER(blinkMode);
	}

    /** Set Contrast
     * setContrast method is supported by some compatible devices (eg ST7032i) that have onboard LCD voltage generation
     * Code imported from fork by JH1PJL
     *
     * @param unsigned char c   contrast data (6 significant bits, valid range 0..63, Value 0 will disable the Vgen)  
     * @return none
     */
	void setContrast(unsigned char c = LCD_DEF_CONTRAST)
	{
		UNREFERENCED_PARAMETER(c);
	}

    /** Set Power
     * setPower method is supported by some compatible devices (eg SSD1803) that have power down modes
     *
     * @param bool powerOn  Power on/off   
     * @return none
     */
	void setPower(bool powerOn = true)
	{
		UNREFERENCED_PARAMETER(powerOn);
	}

    /** Set Orient
     * setOrient method is supported by some compatible devices (eg SSD1803, US2066) that have top/bottom view modes
     *
     * @param LCDOrient orient Orientation 
     * @return none
     */
	void setOrient(LCDOrient orient = Top)
	{
		UNREFERENCED_PARAMETER(orient);
	}


//test
//    void _initCtrl();    
    
protected:

   /** LCD controller select, mainly used for LCD40x4
     */
    enum _LCDCtrl_Idx {
        _LCDCtrl_0,  /*<  Primary */    
        _LCDCtrl_1,  /*<  Secondary */            
    };

    /** Create a TextLCD_Base interface
      * @brief Base class, can not be instantiated
      *
      * @param type  Sets the panel size/addressing mode (default = LCD16x2)
      * @param ctrl  LCD controller (default = HD44780)           
      */
	TextLCD_Base(LCDType type = LCD16x2, LCDCtrl ctrl = HD44780)
	{
		switch (type & LCD_T_ROW_MSK)
		{
		case LCD_T_R1: _rows = 1; break;
		case LCD_T_R2: _rows = 2; break;
		case LCD_T_R3: _rows = 3; break;
		case LCD_T_R4: _rows = 4; break;
		default: _rows = 2; break;
		}
		switch (type & LCD_T_COL_MSK)
		{
		case LCD_T_C6: _cols = 6; break;
		case LCD_T_C8: _cols = 8; break;
		case LCD_T_C10: _cols = 10; break;
		case LCD_T_C12: _cols = 12; break;
		case LCD_T_C16: _cols = 16; break;
		case LCD_T_C20: _cols = 20; break;
		case LCD_T_C24: _cols = 24; break;
		case LCD_T_C32: _cols = 32; break;
		case LCD_T_C40: _cols = 40; break;
		default: _cols = 16; break;
		}
		UNREFERENCED_PARAMETER(ctrl);
		AESIM::textlcd_init(this, _cols, _rows);
	}
    
    // Stream implementation functions
	virtual int _putc(int value)
	{
		AESIM::textlcd_putc(this, value);
		return 0;
	}
	virtual int _getc()
	{
		return EOF;
	}

	private:
		int _rows, _cols;
      
};

//--------- End TextLCD_Base -----------



//--------- Start TextLCD Bus -----------

/** Create a TextLCD interface for using regular mbed pins
  *
  */
class TextLCD : public TextLCD_Base {
public:    
    /** Create a TextLCD interface for using regular mbed pins
     *
     * @param rs    Instruction/data control line
     * @param e     Enable line (clock)
     * @param d4-d7 Data lines for using as a 4-bit interface
     * @param type  Sets the panel size/addressing mode (default = LCD16x2)
     * @param bl    Backlight control line (optional, default = NC)      
     * @param e2    Enable2 line (clock for second controller, LCD40x4 only)  
     * @param ctrl  LCD controller (default = HD44780)           
     */
	TextLCD(PinName rs, PinName e, PinName d4, PinName d5, PinName d6, PinName d7, LCDType type = LCD16x2, PinName bl = NC, PinName e2 = NC, LCDCtrl ctrl = HD44780)
		: TextLCD_Base(type, ctrl)
	{
		// I currently simulate just one TextLCD per mbed
		UNREFERENCED_PARAMETER(rs);
		UNREFERENCED_PARAMETER(e);
		UNREFERENCED_PARAMETER(d4);
		UNREFERENCED_PARAMETER(d5);
		UNREFERENCED_PARAMETER(d6);
		UNREFERENCED_PARAMETER(d7);
		UNREFERENCED_PARAMETER(bl);
		UNREFERENCED_PARAMETER(e2);
	}

   /** Destruct a TextLCD interface for using regular mbed pins
     *
     * @param  none
     * @return none
     */ 
	virtual ~TextLCD() { }

};

    
//----------- End TextLCD ---------------




#endif
