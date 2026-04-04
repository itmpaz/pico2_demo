
#ifndef __FONTS_H
#define __FONTS_H

#include <stdint.h>

#define FONT_PACK_H  0
#define FONT_PACK_V  1


typedef struct _tFont
{    
  const uint8_t *table;
  uint16_t Width;
  uint16_t Height;
  uint8_t Pack;
  
} sFONT;


#endif /* __FONTS_H */
 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
