
#include "crc8.h"

uint8_t crc8(uint8_t *inData, uint8_t len)
{
   uint8_t crc;
   crc = 0;
   for(; len; len--)
   {
      crc ^= *inData++;
      crc ^= (crc << 3) ^ (crc << 4) ^ (crc << 6);
      crc ^= (crc >> 4) ^ (crc >> 5);
   }
   return crc;
}
