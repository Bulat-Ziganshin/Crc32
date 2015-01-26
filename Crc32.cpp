// //////////////////////////////////////////////////////////
// Crc32.cpp
// Copyright (c) 2011-2013 Stephan Brumme. All rights reserved.
// see http://create.stephan-brumme.com/disclaimer.html
//

// g++ -o Crc32 Crc32.cpp -O3 -march=native -mtune=native

#include <stdlib.h>

// define endianess and some integer data types
#include <stdint.h>
  #define __LITTLE_ENDIAN 1234
  #define __BIG_ENDIAN    4321
  #define __BYTE_ORDER    __LITTLE_ENDIAN

/// zlib's CRC32 polynomial
const uint32_t Polynomial = 0xEDB88320;

/// swap endianess
static inline uint32_t swap(uint32_t x)
{
  return (x >> 24) |
        ((x >>  8) & 0x0000FF00) |
        ((x <<  8) & 0x00FF0000) |
         (x << 24);
}

/// forward declaration, table is at the end of this file
uint32_t Crc32Lookup[16][256]; // extern is needed to keep compiler happey


/// compute CRC32 (bitwise algorithm)
uint32_t crc32_bitwise(const void* data, size_t length, uint32_t previousCrc32 = 0)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint8_t* current = (const uint8_t*) data;

  while (length-- > 0)
  {
    crc ^= *current++;

    for (int j = 0; j < 8; j++)
    {
      // branch-free, version 1
      //crc = (crc >> 1) ^ (crc & 1)*Polynomial;
      // branch-free, version 2
      crc = (crc >> 1) ^ (-int32_t(crc & 1) & Polynomial);

      // branching, much slower:
      //if (crc & 1)
      //  crc = (crc >> 1) ^ Polynomial;
      //else
      //  crc =  crc >> 1;
    }
  }

  return ~crc; // same as crc ^ 0xFFFFFFFF
}


/// compute CRC32 (half-byte algoritm)
uint32_t crc32_halfbyte(const void* data, size_t length, uint32_t previousCrc32 = 0)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint8_t* current = (const uint8_t*) data;

  /// look-up table for half-byte, same as crc32Lookup[0][16*i]
  static const uint32_t Crc32Lookup16[16] =
  {
    0x00000000,0x1DB71064,0x3B6E20C8,0x26D930AC,0x76DC4190,0x6B6B51F4,0x4DB26158,0x5005713C,
    0xEDB88320,0xF00F9344,0xD6D6A3E8,0xCB61B38C,0x9B64C2B0,0x86D3D2D4,0xA00AE278,0xBDBDF21C
  };

  while (length-- > 0)
  {
    crc = Crc32Lookup16[(crc ^  *current      ) & 0x0F] ^ (crc >> 4);
    crc = Crc32Lookup16[(crc ^ (*current >> 4)) & 0x0F] ^ (crc >> 4);
    current++;
  }

  return ~crc; // same as crc ^ 0xFFFFFFFF
}


/// compute CRC32 (standard algorithm)
uint32_t crc32_1byte(const void* data, size_t length, uint32_t previousCrc32 = 0)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint8_t* current = (const uint8_t*) data;

  while (length-- > 0)
    crc = (crc >> 8) ^ Crc32Lookup[0][(crc & 0xFF) ^ *current++];

  return ~crc; // same as crc ^ 0xFFFFFFFF
}


/// compute CRC32 (Slicing-by-4 algorithm)
uint32_t crc32_4bytes(const void* data, size_t length, uint32_t previousCrc32 = 0)
{
  uint32_t  crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint32_t* current = (const uint32_t*) data;

  // process four bytes at once (Slicing-by-4)
  while (length >= 4)
  {
{
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one = *current++ ^ swap(crc);
    crc  = Crc32Lookup[0][ one      & 0xFF] ^
           Crc32Lookup[1][(one>> 8) & 0xFF] ^
           Crc32Lookup[2][(one>>16) & 0xFF] ^
           Crc32Lookup[3][(one>>24) & 0xFF];
#else
    uint32_t one = *current++ ^ crc;
    crc  = Crc32Lookup[0][(one>>24) & 0xFF] ^
           Crc32Lookup[1][(one>>16) & 0xFF] ^
           Crc32Lookup[2][(one>> 8) & 0xFF] ^
           Crc32Lookup[3][ one      & 0xFF];
#endif

}
    length -= 4;
  }

  const uint8_t* currentChar = (const uint8_t*) current;
  // remaining 1 to 3 bytes (standard algorithm)
  while (length-- > 0)
    crc = (crc >> 8) ^ Crc32Lookup[0][(crc & 0xFF) ^ *currentChar++];

  return ~crc; // same as crc ^ 0xFFFFFFFF
}


/// compute CRC32 (Slicing-by-4 algorithm)
uint32_t crc32_2x4bytes(const void* data, size_t length, uint32_t previousCrc32 = 0)
{
  uint32_t  crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint32_t* current = (const uint32_t*) data;

  // process four bytes at once (Slicing-by-4)
  while (length >= 8)
  {
{
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one = *current++ ^ swap(crc);
    crc  = Crc32Lookup[0][ one      & 0xFF] ^
           Crc32Lookup[1][(one>> 8) & 0xFF] ^
           Crc32Lookup[2][(one>>16) & 0xFF] ^
           Crc32Lookup[3][(one>>24) & 0xFF];
#else
    uint32_t one = *current++ ^ crc;
    crc  = Crc32Lookup[0][(one>>24) & 0xFF] ^
           Crc32Lookup[1][(one>>16) & 0xFF] ^
           Crc32Lookup[2][(one>> 8) & 0xFF] ^
           Crc32Lookup[3][ one      & 0xFF];
#endif

}
{
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one = *current++ ^ swap(crc);
    crc  = Crc32Lookup[0][ one      & 0xFF] ^
           Crc32Lookup[1][(one>> 8) & 0xFF] ^
           Crc32Lookup[2][(one>>16) & 0xFF] ^
           Crc32Lookup[3][(one>>24) & 0xFF];
#else
    uint32_t one = *current++ ^ crc;
    crc  = Crc32Lookup[0][(one>>24) & 0xFF] ^
           Crc32Lookup[1][(one>>16) & 0xFF] ^
           Crc32Lookup[2][(one>> 8) & 0xFF] ^
           Crc32Lookup[3][ one      & 0xFF];
#endif

}
    length -= 8;
  }

  const uint8_t* currentChar = (const uint8_t*) current;
  // remaining 1 to 3 bytes (standard algorithm)
  while (length-- > 0)
    crc = (crc >> 8) ^ Crc32Lookup[0][(crc & 0xFF) ^ *currentChar++];

  return ~crc; // same as crc ^ 0xFFFFFFFF
}


/// compute CRC32 (Slicing-by-4 algorithm)
uint32_t crc32_4x4bytes(const void* data, size_t length, uint32_t previousCrc32 = 0)
{
  uint32_t  crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint32_t* current = (const uint32_t*) data;

  // process four bytes at once (Slicing-by-4)
  while (length >= 16)
  {
{
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one = *current++ ^ swap(crc);
    crc  = Crc32Lookup[0][ one      & 0xFF] ^
           Crc32Lookup[1][(one>> 8) & 0xFF] ^
           Crc32Lookup[2][(one>>16) & 0xFF] ^
           Crc32Lookup[3][(one>>24) & 0xFF];
#else
    uint32_t one = *current++ ^ crc;
    crc  = Crc32Lookup[0][(one>>24) & 0xFF] ^
           Crc32Lookup[1][(one>>16) & 0xFF] ^
           Crc32Lookup[2][(one>> 8) & 0xFF] ^
           Crc32Lookup[3][ one      & 0xFF];
#endif

}
{
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one = *current++ ^ swap(crc);
    crc  = Crc32Lookup[0][ one      & 0xFF] ^
           Crc32Lookup[1][(one>> 8) & 0xFF] ^
           Crc32Lookup[2][(one>>16) & 0xFF] ^
           Crc32Lookup[3][(one>>24) & 0xFF];
#else
    uint32_t one = *current++ ^ crc;
    crc  = Crc32Lookup[0][(one>>24) & 0xFF] ^
           Crc32Lookup[1][(one>>16) & 0xFF] ^
           Crc32Lookup[2][(one>> 8) & 0xFF] ^
           Crc32Lookup[3][ one      & 0xFF];
#endif

}
{
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one = *current++ ^ swap(crc);
    crc  = Crc32Lookup[0][ one      & 0xFF] ^
           Crc32Lookup[1][(one>> 8) & 0xFF] ^
           Crc32Lookup[2][(one>>16) & 0xFF] ^
           Crc32Lookup[3][(one>>24) & 0xFF];
#else
    uint32_t one = *current++ ^ crc;
    crc  = Crc32Lookup[0][(one>>24) & 0xFF] ^
           Crc32Lookup[1][(one>>16) & 0xFF] ^
           Crc32Lookup[2][(one>> 8) & 0xFF] ^
           Crc32Lookup[3][ one      & 0xFF];
#endif

}
{
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one = *current++ ^ swap(crc);
    crc  = Crc32Lookup[0][ one      & 0xFF] ^
           Crc32Lookup[1][(one>> 8) & 0xFF] ^
           Crc32Lookup[2][(one>>16) & 0xFF] ^
           Crc32Lookup[3][(one>>24) & 0xFF];
#else
    uint32_t one = *current++ ^ crc;
    crc  = Crc32Lookup[0][(one>>24) & 0xFF] ^
           Crc32Lookup[1][(one>>16) & 0xFF] ^
           Crc32Lookup[2][(one>> 8) & 0xFF] ^
           Crc32Lookup[3][ one      & 0xFF];
#endif

}
    length -= 16;
  }

  const uint8_t* currentChar = (const uint8_t*) current;
  // remaining 1 to 3 bytes (standard algorithm)
  while (length-- > 0)
    crc = (crc >> 8) ^ Crc32Lookup[0][(crc & 0xFF) ^ *currentChar++];

  return ~crc; // same as crc ^ 0xFFFFFFFF
}


/// compute CRC32 (Slicing-by-8 algorithm) ///////////////////////////////////////////////////////////////////////////////////////////
uint32_t crc32_88bytes(const void* data, size_t length, uint32_t previousCrc32 = 0)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint32_t* current = (const uint32_t*) data;

  // process eight bytes at once (Slicing-by-8)
  while (length >= 16)
  {
{
    uint32_t one = current[0] ^ crc;
    uint32_t two = current[1];
    uint32_t a1 = Crc32Lookup[0][(two>>24) & 0xFF] ^
                  Crc32Lookup[1][(two>>16) & 0xFF];
    uint32_t a2 = Crc32Lookup[2][(two>> 8) & 0xFF] ^
                  Crc32Lookup[3][ two      & 0xFF];
    uint32_t a3 = Crc32Lookup[4][(one>>24) & 0xFF] ^
                  Crc32Lookup[5][(one>>16) & 0xFF];
    uint32_t a4 = Crc32Lookup[6][(one>> 8) & 0xFF] ^
                  Crc32Lookup[7][ one      & 0xFF];

    crc  = (a1^a2)^(a3^a4);
}
    uint32_t two = current[3];
    uint32_t a1 = Crc32Lookup[0][(two>>24) & 0xFF] ^
                  Crc32Lookup[1][(two>>16) & 0xFF];
    uint32_t a2 = Crc32Lookup[2][(two>> 8) & 0xFF] ^
                  Crc32Lookup[3][ two      & 0xFF];
{
    uint32_t one = current[2] ^ crc;
    current += 4;
    uint32_t a3 = Crc32Lookup[4][(one>>24) & 0xFF] ^
                  Crc32Lookup[5][(one>>16) & 0xFF];
    uint32_t a4 = Crc32Lookup[6][(one>> 8) & 0xFF] ^
                  Crc32Lookup[7][ one      & 0xFF];

    crc  = (a1^a2)^(a3^a4);
}

    length -= 16;
  }

  const uint8_t* currentChar = (const uint8_t*) current;
  // remaining 1 to 7 bytes (standard algorithm)
  while (length-- > 0)
    crc = (crc >> 8) ^ Crc32Lookup[0][(crc & 0xFF) ^ *currentChar++];

  return ~crc; // same as crc ^ 0xFFFFFFFF
}


/// compute CRC32 (Slicing-by-8 algorithm)
uint32_t crc32_8bytes(const void* data, size_t length, uint32_t previousCrc32 = 0)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint32_t* current = (const uint32_t*) data;

  // process eight bytes at once (Slicing-by-8)
  while (length >= 8)
  {
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one = *current++ ^ swap(crc);
    uint32_t two = *current++;
    crc  = Crc32Lookup[0][ two      & 0xFF] ^
           Crc32Lookup[1][(two>> 8) & 0xFF] ^
           Crc32Lookup[2][(two>>16) & 0xFF] ^
           Crc32Lookup[3][(two>>24) & 0xFF] ^
           Crc32Lookup[4][ one      & 0xFF] ^
           Crc32Lookup[5][(one>> 8) & 0xFF] ^
           Crc32Lookup[6][(one>>16) & 0xFF] ^
           Crc32Lookup[7][(one>>24) & 0xFF];
#else
    uint32_t one = *current++ ^ crc;
    uint32_t two = *current++;
    crc  = Crc32Lookup[0][(two>>24) & 0xFF] ^
           Crc32Lookup[1][(two>>16) & 0xFF] ^
           Crc32Lookup[2][(two>> 8) & 0xFF] ^
           Crc32Lookup[3][ two      & 0xFF] ^
           Crc32Lookup[4][(one>>24) & 0xFF] ^
           Crc32Lookup[5][(one>>16) & 0xFF] ^
           Crc32Lookup[6][(one>> 8) & 0xFF] ^
           Crc32Lookup[7][ one      & 0xFF];
#endif

    length -= 8;
  }

  const uint8_t* currentChar = (const uint8_t*) current;
  // remaining 1 to 7 bytes (standard algorithm)
  while (length-- > 0)
    crc = (crc >> 8) ^ Crc32Lookup[0][(crc & 0xFF) ^ *currentChar++];

  return ~crc; // same as crc ^ 0xFFFFFFFF
}


/// compute CRC32 (Slicing-by-8 algorithm) //////////////////////////////////////////////////////////////////////////////////////////
uint32_t crc32_16bytes(const void* data, size_t length, uint32_t previousCrc32 = 0)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint32_t* current = (const uint32_t*) data;

  // process eight bytes at once (Slicing-by-8)
  while (length >= 16)
  {
    uint32_t one = *current++ ^ crc;
    uint32_t two = *current++;
    uint32_t a3  = *current++;
    uint32_t a4  = *current++;
    crc  = Crc32Lookup[ 0][( a4>>24) & 0xFF] ^
           Crc32Lookup[ 1][( a4>>16) & 0xFF] ^
           Crc32Lookup[ 2][( a4>> 8) & 0xFF] ^
           Crc32Lookup[ 3][  a4      & 0xFF] ^
           Crc32Lookup[ 4][( a3>>24) & 0xFF] ^
           Crc32Lookup[ 5][( a3>>16) & 0xFF] ^
           Crc32Lookup[ 6][( a3>> 8) & 0xFF] ^
           Crc32Lookup[ 7][  a3      & 0xFF] ^
           Crc32Lookup[ 8][(two>>24) & 0xFF] ^
           Crc32Lookup[ 9][(two>>16) & 0xFF] ^
           Crc32Lookup[10][(two>> 8) & 0xFF] ^
           Crc32Lookup[11][ two      & 0xFF] ^
           Crc32Lookup[12][(one>>24) & 0xFF] ^
           Crc32Lookup[13][(one>>16) & 0xFF] ^
           Crc32Lookup[14][(one>> 8) & 0xFF] ^
           Crc32Lookup[15][ one      & 0xFF];

    length -= 16;
  }

  const uint8_t* currentChar = (const uint8_t*) current;
  // remaining 1 to 7 bytes (standard algorithm)
  while (length-- > 0)
    crc = (crc >> 8) ^ Crc32Lookup[0][(crc & 0xFF) ^ *currentChar++];

  return ~crc; // same as crc ^ 0xFFFFFFFF
}


/// compute CRC32 (Slicing-by-8 algorithm) //////////////////////////////////////////////////////////////////////////////////////////
uint32_t crc32_2x16bytes(const void* data, size_t length, uint32_t previousCrc32 = 0)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint32_t* current = (const uint32_t*) data;

  // process eight bytes at once (Slicing-by-8)
  while (length >= 32)
  {
{
    uint32_t one = *current++ ^ crc;
    uint32_t two = *current++;
    uint32_t a3  = *current++;
    uint32_t a4  = *current++;
    crc  = Crc32Lookup[ 0][( a4>>24) & 0xFF] ^
           Crc32Lookup[ 1][( a4>>16) & 0xFF] ^
           Crc32Lookup[ 2][( a4>> 8) & 0xFF] ^
           Crc32Lookup[ 3][  a4      & 0xFF] ^
           Crc32Lookup[ 4][( a3>>24) & 0xFF] ^
           Crc32Lookup[ 5][( a3>>16) & 0xFF] ^
           Crc32Lookup[ 6][( a3>> 8) & 0xFF] ^
           Crc32Lookup[ 7][  a3      & 0xFF] ^
           Crc32Lookup[ 8][(two>>24) & 0xFF] ^
           Crc32Lookup[ 9][(two>>16) & 0xFF] ^
           Crc32Lookup[10][(two>> 8) & 0xFF] ^
           Crc32Lookup[11][ two      & 0xFF] ^
           Crc32Lookup[12][(one>>24) & 0xFF] ^
           Crc32Lookup[13][(one>>16) & 0xFF] ^
           Crc32Lookup[14][(one>> 8) & 0xFF] ^
           Crc32Lookup[15][ one      & 0xFF];
}
{
    uint32_t one = *current++ ^ crc;
    uint32_t two = *current++;
    uint32_t a3  = *current++;
    uint32_t a4  = *current++;
    crc  = Crc32Lookup[ 0][( a4>>24) & 0xFF] ^
           Crc32Lookup[ 1][( a4>>16) & 0xFF] ^
           Crc32Lookup[ 2][( a4>> 8) & 0xFF] ^
           Crc32Lookup[ 3][  a4      & 0xFF] ^
           Crc32Lookup[ 4][( a3>>24) & 0xFF] ^
           Crc32Lookup[ 5][( a3>>16) & 0xFF] ^
           Crc32Lookup[ 6][( a3>> 8) & 0xFF] ^
           Crc32Lookup[ 7][  a3      & 0xFF] ^
           Crc32Lookup[ 8][(two>>24) & 0xFF] ^
           Crc32Lookup[ 9][(two>>16) & 0xFF] ^
           Crc32Lookup[10][(two>> 8) & 0xFF] ^
           Crc32Lookup[11][ two      & 0xFF] ^
           Crc32Lookup[12][(one>>24) & 0xFF] ^
           Crc32Lookup[13][(one>>16) & 0xFF] ^
           Crc32Lookup[14][(one>> 8) & 0xFF] ^
           Crc32Lookup[15][ one      & 0xFF];
}
    length -= 32;
  }

  const uint8_t* currentChar = (const uint8_t*) current;
  // remaining 1 to 7 bytes (standard algorithm)
  while (length-- > 0)
    crc = (crc >> 8) ^ Crc32Lookup[0][(crc & 0xFF) ^ *currentChar++];

  return ~crc; // same as crc ^ 0xFFFFFFFF
}


/// compute CRC32 (Slicing-by-8 algorithm)
uint32_t crc32_2x8bytes(const void* data, size_t length, uint32_t previousCrc32 = 0)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint32_t* current = (const uint32_t*) data;

  // process eight bytes at once (Slicing-by-8)
  while (length >= 16)
  {
{
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one = *current++ ^ swap(crc);
    uint32_t two = *current++;
    crc  = Crc32Lookup[0][ two      & 0xFF] ^
           Crc32Lookup[1][(two>> 8) & 0xFF] ^
           Crc32Lookup[2][(two>>16) & 0xFF] ^
           Crc32Lookup[3][(two>>24) & 0xFF] ^
           Crc32Lookup[4][ one      & 0xFF] ^
           Crc32Lookup[5][(one>> 8) & 0xFF] ^
           Crc32Lookup[6][(one>>16) & 0xFF] ^
           Crc32Lookup[7][(one>>24) & 0xFF];
#else
    uint32_t one = *current++ ^ crc;
    uint32_t two = *current++;
    crc  = Crc32Lookup[0][(two>>24) & 0xFF] ^
           Crc32Lookup[1][(two>>16) & 0xFF] ^
           Crc32Lookup[2][(two>> 8) & 0xFF] ^
           Crc32Lookup[3][ two      & 0xFF] ^
           Crc32Lookup[4][(one>>24) & 0xFF] ^
           Crc32Lookup[5][(one>>16) & 0xFF] ^
           Crc32Lookup[6][(one>> 8) & 0xFF] ^
           Crc32Lookup[7][ one      & 0xFF];
#endif
}
{
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one = *current++ ^ swap(crc);
    uint32_t two = *current++;
    crc  = Crc32Lookup[0][ two      & 0xFF] ^
           Crc32Lookup[1][(two>> 8) & 0xFF] ^
           Crc32Lookup[2][(two>>16) & 0xFF] ^
           Crc32Lookup[3][(two>>24) & 0xFF] ^
           Crc32Lookup[4][ one      & 0xFF] ^
           Crc32Lookup[5][(one>> 8) & 0xFF] ^
           Crc32Lookup[6][(one>>16) & 0xFF] ^
           Crc32Lookup[7][(one>>24) & 0xFF];
#else
    uint32_t one = *current++ ^ crc;
    uint32_t two = *current++;
    crc  = Crc32Lookup[0][(two>>24) & 0xFF] ^
           Crc32Lookup[1][(two>>16) & 0xFF] ^
           Crc32Lookup[2][(two>> 8) & 0xFF] ^
           Crc32Lookup[3][ two      & 0xFF] ^
           Crc32Lookup[4][(one>>24) & 0xFF] ^
           Crc32Lookup[5][(one>>16) & 0xFF] ^
           Crc32Lookup[6][(one>> 8) & 0xFF] ^
           Crc32Lookup[7][ one      & 0xFF];
#endif
}
    length -= 16;
  }

  const uint8_t* currentChar = (const uint8_t*) current;
  // remaining 1 to 7 bytes (standard algorithm)
  while (length-- > 0)
    crc = (crc >> 8) ^ Crc32Lookup[0][(crc & 0xFF) ^ *currentChar++];

  return ~crc; // same as crc ^ 0xFFFFFFFF
}


/// compute CRC32 (Slicing-by-8 algorithm)
uint32_t crc32_4x8bytes(const void* data, size_t length, uint32_t previousCrc32 = 0)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint32_t* current = (const uint32_t*) data;

  // process eight bytes at once (Slicing-by-8)
  while (length >= 32)
  {
{
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one = *current++ ^ swap(crc);
    uint32_t two = *current++;
    crc  = Crc32Lookup[0][ two      & 0xFF] ^
           Crc32Lookup[1][(two>> 8) & 0xFF] ^
           Crc32Lookup[2][(two>>16) & 0xFF] ^
           Crc32Lookup[3][(two>>24) & 0xFF] ^
           Crc32Lookup[4][ one      & 0xFF] ^
           Crc32Lookup[5][(one>> 8) & 0xFF] ^
           Crc32Lookup[6][(one>>16) & 0xFF] ^
           Crc32Lookup[7][(one>>24) & 0xFF];
#else
    uint32_t one = *current++ ^ crc;
    uint32_t two = *current++;
    crc  = Crc32Lookup[0][(two>>24) & 0xFF] ^
           Crc32Lookup[1][(two>>16) & 0xFF] ^
           Crc32Lookup[2][(two>> 8) & 0xFF] ^
           Crc32Lookup[3][ two      & 0xFF] ^
           Crc32Lookup[4][(one>>24) & 0xFF] ^
           Crc32Lookup[5][(one>>16) & 0xFF] ^
           Crc32Lookup[6][(one>> 8) & 0xFF] ^
           Crc32Lookup[7][ one      & 0xFF];
#endif
}
{
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one = *current++ ^ swap(crc);
    uint32_t two = *current++;
    crc  = Crc32Lookup[0][ two      & 0xFF] ^
           Crc32Lookup[1][(two>> 8) & 0xFF] ^
           Crc32Lookup[2][(two>>16) & 0xFF] ^
           Crc32Lookup[3][(two>>24) & 0xFF] ^
           Crc32Lookup[4][ one      & 0xFF] ^
           Crc32Lookup[5][(one>> 8) & 0xFF] ^
           Crc32Lookup[6][(one>>16) & 0xFF] ^
           Crc32Lookup[7][(one>>24) & 0xFF];
#else
    uint32_t one = *current++ ^ crc;
    uint32_t two = *current++;
    crc  = Crc32Lookup[0][(two>>24) & 0xFF] ^
           Crc32Lookup[1][(two>>16) & 0xFF] ^
           Crc32Lookup[2][(two>> 8) & 0xFF] ^
           Crc32Lookup[3][ two      & 0xFF] ^
           Crc32Lookup[4][(one>>24) & 0xFF] ^
           Crc32Lookup[5][(one>>16) & 0xFF] ^
           Crc32Lookup[6][(one>> 8) & 0xFF] ^
           Crc32Lookup[7][ one      & 0xFF];
#endif
}
{
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one = *current++ ^ swap(crc);
    uint32_t two = *current++;
    crc  = Crc32Lookup[0][ two      & 0xFF] ^
           Crc32Lookup[1][(two>> 8) & 0xFF] ^
           Crc32Lookup[2][(two>>16) & 0xFF] ^
           Crc32Lookup[3][(two>>24) & 0xFF] ^
           Crc32Lookup[4][ one      & 0xFF] ^
           Crc32Lookup[5][(one>> 8) & 0xFF] ^
           Crc32Lookup[6][(one>>16) & 0xFF] ^
           Crc32Lookup[7][(one>>24) & 0xFF];
#else
    uint32_t one = *current++ ^ crc;
    uint32_t two = *current++;
    crc  = Crc32Lookup[0][(two>>24) & 0xFF] ^
           Crc32Lookup[1][(two>>16) & 0xFF] ^
           Crc32Lookup[2][(two>> 8) & 0xFF] ^
           Crc32Lookup[3][ two      & 0xFF] ^
           Crc32Lookup[4][(one>>24) & 0xFF] ^
           Crc32Lookup[5][(one>>16) & 0xFF] ^
           Crc32Lookup[6][(one>> 8) & 0xFF] ^
           Crc32Lookup[7][ one      & 0xFF];
#endif
}
{
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one = *current++ ^ swap(crc);
    uint32_t two = *current++;
    crc  = Crc32Lookup[0][ two      & 0xFF] ^
           Crc32Lookup[1][(two>> 8) & 0xFF] ^
           Crc32Lookup[2][(two>>16) & 0xFF] ^
           Crc32Lookup[3][(two>>24) & 0xFF] ^
           Crc32Lookup[4][ one      & 0xFF] ^
           Crc32Lookup[5][(one>> 8) & 0xFF] ^
           Crc32Lookup[6][(one>>16) & 0xFF] ^
           Crc32Lookup[7][(one>>24) & 0xFF];
#else
    uint32_t one = *current++ ^ crc;
    uint32_t two = *current++;
    crc  = Crc32Lookup[0][(two>>24) & 0xFF] ^
           Crc32Lookup[1][(two>>16) & 0xFF] ^
           Crc32Lookup[2][(two>> 8) & 0xFF] ^
           Crc32Lookup[3][ two      & 0xFF] ^
           Crc32Lookup[4][(one>>24) & 0xFF] ^
           Crc32Lookup[5][(one>>16) & 0xFF] ^
           Crc32Lookup[6][(one>> 8) & 0xFF] ^
           Crc32Lookup[7][ one      & 0xFF];
#endif
}
    length -= 32;
  }

  const uint8_t* currentChar = (const uint8_t*) current;
  // remaining 1 to 7 bytes (standard algorithm)
  while (length-- > 0)
    crc = (crc >> 8) ^ Crc32Lookup[0][(crc & 0xFF) ^ *currentChar++];

  return ~crc; // same as crc ^ 0xFFFFFFFF
}

#include "crc32ctables.cc"

uint32_t crc32cSlicingBy4(const void* data, size_t length, uint32_t crc) {
    const char* p_buf = (const char*) data;

    // Handle leading misaligned bytes
    size_t initial_bytes = (sizeof(int32_t) - (intptr_t)p_buf) & (sizeof(int32_t) - 1);
    if (length < initial_bytes) initial_bytes = length;
    for (size_t li = 0; li < initial_bytes; li++) {
        crc = crc_tableil8_o32[(crc ^ *p_buf++) & 0x000000FF] ^ (crc >> 8);
    }

    length -= initial_bytes;
    size_t running_length = length & ~(sizeof(int32_t) - 1);
    size_t end_bytes = length - running_length;

    for (size_t li = 0; li < running_length/4; li++) {
{
        crc ^= *(uint32_t*) p_buf;
        p_buf += 4;
        uint32_t term1 = crc_tableil8_o56[crc & 0x000000FF] ^
                crc_tableil8_o48[(crc >> 8) & 0x000000FF];
        uint32_t term2 = crc >> 16;
        crc = term1 ^
              crc_tableil8_o40[term2 & 0x000000FF] ^
              crc_tableil8_o32[(term2 >> 8) & 0x000000FF];
}
    }

    for (size_t li=0; li < end_bytes; li++) {
        crc = crc_tableil8_o32[(crc ^ *p_buf++) & 0x000000FF] ^ (crc >> 8);
    }

    return crc;
}


uint32_t crc32cSlicingBy2x4(const void* data, size_t length, uint32_t crc) {
    const char* p_buf = (const char*) data;

    // Handle leading misaligned bytes
    size_t initial_bytes = (sizeof(int32_t) - (intptr_t)p_buf) & (sizeof(int32_t) - 1);
    if (length < initial_bytes) initial_bytes = length;
    for (size_t li = 0; li < initial_bytes; li++) {
        crc = crc_tableil8_o32[(crc ^ *p_buf++) & 0x000000FF] ^ (crc >> 8);
    }

    length -= initial_bytes;
    size_t running_length = length & ~(sizeof(int32_t) - 1);
    size_t end_bytes = length - running_length;

    for (size_t li = 0; li < running_length/8; li++) {
{
        crc ^= *(uint32_t*) p_buf;
        p_buf += 4;
        uint32_t term1 = crc_tableil8_o56[crc & 0x000000FF] ^
                crc_tableil8_o48[(crc >> 8) & 0x000000FF];
        uint32_t term2 = crc >> 16;
        crc = term1 ^
              crc_tableil8_o40[term2 & 0x000000FF] ^
              crc_tableil8_o32[(term2 >> 8) & 0x000000FF];
}
{
        crc ^= *(uint32_t*) p_buf;
        p_buf += 4;
        uint32_t term1 = crc_tableil8_o56[crc & 0x000000FF] ^
                crc_tableil8_o48[(crc >> 8) & 0x000000FF];
        uint32_t term2 = crc >> 16;
        crc = term1 ^
              crc_tableil8_o40[term2 & 0x000000FF] ^
              crc_tableil8_o32[(term2 >> 8) & 0x000000FF];
}
    }

    for (size_t li=0; li < end_bytes; li++) {
        crc = crc_tableil8_o32[(crc ^ *p_buf++) & 0x000000FF] ^ (crc >> 8);
    }

    return crc;
}


uint32_t crc32cSlicingBy4x4(const void* data, size_t length, uint32_t crc) {
    const char* p_buf = (const char*) data;

    // Handle leading misaligned bytes
    size_t initial_bytes = (sizeof(int32_t) - (intptr_t)p_buf) & (sizeof(int32_t) - 1);
    if (length < initial_bytes) initial_bytes = length;
    for (size_t li = 0; li < initial_bytes; li++) {
        crc = crc_tableil8_o32[(crc ^ *p_buf++) & 0x000000FF] ^ (crc >> 8);
    }

    length -= initial_bytes;
    size_t running_length = length & ~(sizeof(int32_t) - 1);
    size_t end_bytes = length - running_length;

    for (size_t li = 0; li < running_length/16; li++) {
{
        crc ^= *(uint32_t*) p_buf;
        p_buf += 4;
        uint32_t term1 = crc_tableil8_o56[crc & 0x000000FF] ^
                crc_tableil8_o48[(crc >> 8) & 0x000000FF];
        uint32_t term2 = crc >> 16;
        crc = term1 ^
              crc_tableil8_o40[term2 & 0x000000FF] ^
              crc_tableil8_o32[(term2 >> 8) & 0x000000FF];
}
{
        crc ^= *(uint32_t*) p_buf;
        p_buf += 4;
        uint32_t term1 = crc_tableil8_o56[crc & 0x000000FF] ^
                crc_tableil8_o48[(crc >> 8) & 0x000000FF];
        uint32_t term2 = crc >> 16;
        crc = term1 ^
              crc_tableil8_o40[term2 & 0x000000FF] ^
              crc_tableil8_o32[(term2 >> 8) & 0x000000FF];
}
{
        crc ^= *(uint32_t*) p_buf;
        p_buf += 4;
        uint32_t term1 = crc_tableil8_o56[crc & 0x000000FF] ^
                crc_tableil8_o48[(crc >> 8) & 0x000000FF];
        uint32_t term2 = crc >> 16;
        crc = term1 ^
              crc_tableil8_o40[term2 & 0x000000FF] ^
              crc_tableil8_o32[(term2 >> 8) & 0x000000FF];
}
{
        crc ^= *(uint32_t*) p_buf;
        p_buf += 4;
        uint32_t term1 = crc_tableil8_o56[crc & 0x000000FF] ^
                crc_tableil8_o48[(crc >> 8) & 0x000000FF];
        uint32_t term2 = crc >> 16;
        crc = term1 ^
              crc_tableil8_o40[term2 & 0x000000FF] ^
              crc_tableil8_o32[(term2 >> 8) & 0x000000FF];
}
    }

    for (size_t li=0; li < end_bytes; li++) {
        crc = crc_tableil8_o32[(crc ^ *p_buf++) & 0x000000FF] ^ (crc >> 8);
    }

    return crc;
}


uint32_t crc32cSlicingBy8(const void* data, size_t length, uint32_t crc) {
    const char* p_buf = (const char*) data;

    // Handle leading misaligned bytes
    size_t initial_bytes = (sizeof(int32_t) - (intptr_t)p_buf) & (sizeof(int32_t) - 1);
    if (length < initial_bytes) initial_bytes = length;
    for (size_t li = 0; li < initial_bytes; li++) {
        crc = crc_tableil8_o32[(crc ^ *p_buf++) & 0x000000FF] ^ (crc >> 8);
    }

    length -= initial_bytes;
    size_t running_length = length & ~(sizeof(uint64_t) - 1);
    size_t end_bytes = length - running_length;

    for (size_t li = 0; li < running_length/8; li++) {
{
        crc ^= *(uint32_t*) p_buf;
        p_buf += 4;
        uint32_t term1 = crc_tableil8_o88[crc & 0x000000FF] ^
                crc_tableil8_o80[(crc >> 8) & 0x000000FF];
        uint32_t term2 = crc >> 16;
        crc = term1 ^
              crc_tableil8_o72[term2 & 0x000000FF] ^
              crc_tableil8_o64[(term2 >> 8) & 0x000000FF];
        term1 = crc_tableil8_o56[(*(uint32_t *)p_buf) & 0x000000FF] ^
                crc_tableil8_o48[((*(uint32_t *)p_buf) >> 8) & 0x000000FF];

        term2 = (*(uint32_t *)p_buf) >> 16;
        crc = crc ^ term1 ^
                crc_tableil8_o40[term2  & 0x000000FF] ^
                crc_tableil8_o32[(term2 >> 8) & 0x000000FF];
        p_buf += 4;
}
    }

    for (size_t li=0; li < end_bytes; li++) {
        crc = crc_tableil8_o32[(crc ^ *p_buf++) & 0x000000FF] ^ (crc >> 8);
    }

    return crc;
}

uint32_t crc32cSlicingBy16(const void* data, size_t length, uint32_t crc) {
    const char* p_buf = (const char*) data;

    // Handle leading misaligned bytes
    size_t initial_bytes = (sizeof(int32_t) - (intptr_t)p_buf) & (sizeof(int32_t) - 1);
    if (length < initial_bytes) initial_bytes = length;
    for (size_t li = 0; li < initial_bytes; li++) {
        crc = crc_tableil8_o32[(crc ^ *p_buf++) & 0x000000FF] ^ (crc >> 8);
    }

    length -= initial_bytes;
    size_t running_length = length & ~(sizeof(uint64_t) - 1);
    size_t end_bytes = length - running_length;

    for (size_t li = 0; li < running_length/16; li++) {
{
        crc ^= *(uint32_t*) p_buf;
        p_buf += 4;
        uint32_t term1 = crc_tableil8_o88[crc & 0x000000FF] ^
                crc_tableil8_o80[(crc >> 8) & 0x000000FF];
        uint32_t term2 = crc >> 16;
        crc = term1 ^
              crc_tableil8_o72[term2 & 0x000000FF] ^
              crc_tableil8_o64[(term2 >> 8) & 0x000000FF];
        term1 = crc_tableil8_o56[(*(uint32_t *)p_buf) & 0x000000FF] ^
                crc_tableil8_o48[((*(uint32_t *)p_buf) >> 8) & 0x000000FF];

        term2 = (*(uint32_t *)p_buf) >> 16;
        crc = crc ^ term1 ^
                crc_tableil8_o40[term2  & 0x000000FF] ^
                crc_tableil8_o32[(term2 >> 8) & 0x000000FF];
        p_buf += 4;
}
{
        crc ^= *(uint32_t*) p_buf;
        p_buf += 4;
        uint32_t term1 = crc_tableil8_o88[crc & 0x000000FF] ^
                crc_tableil8_o80[(crc >> 8) & 0x000000FF];
        uint32_t term2 = crc >> 16;
        crc = term1 ^
              crc_tableil8_o72[term2 & 0x000000FF] ^
              crc_tableil8_o64[(term2 >> 8) & 0x000000FF];
        term1 = crc_tableil8_o56[(*(uint32_t *)p_buf) & 0x000000FF] ^
                crc_tableil8_o48[((*(uint32_t *)p_buf) >> 8) & 0x000000FF];

        term2 = (*(uint32_t *)p_buf) >> 16;
        crc = crc ^ term1 ^
                crc_tableil8_o40[term2  & 0x000000FF] ^
                crc_tableil8_o32[(term2 >> 8) & 0x000000FF];
        p_buf += 4;
}
    }

    for (size_t li=0; li < end_bytes; li++) {
        crc = crc_tableil8_o32[(crc ^ *p_buf++) & 0x000000FF] ^ (crc >> 8);
    }

    return crc;
}

uint32_t crc32cSlicingBy32(const void* data, size_t length, uint32_t crc) {
    const char* p_buf = (const char*) data;

    // Handle leading misaligned bytes
    size_t initial_bytes = (sizeof(int32_t) - (intptr_t)p_buf) & (sizeof(int32_t) - 1);
    if (length < initial_bytes) initial_bytes = length;
    for (size_t li = 0; li < initial_bytes; li++) {
        crc = crc_tableil8_o32[(crc ^ *p_buf++) & 0x000000FF] ^ (crc >> 8);
    }

    length -= initial_bytes;
    size_t running_length = length & ~(sizeof(uint64_t) - 1);
    size_t end_bytes = length - running_length;

    for (size_t li = 0; li < running_length/32; li++) {
{
        crc ^= *(uint32_t*) p_buf;
        p_buf += 4;
        uint32_t term1 = crc_tableil8_o88[crc & 0x000000FF] ^
                crc_tableil8_o80[(crc >> 8) & 0x000000FF];
        uint32_t term2 = crc >> 16;
        crc = term1 ^
              crc_tableil8_o72[term2 & 0x000000FF] ^
              crc_tableil8_o64[(term2 >> 8) & 0x000000FF];
        term1 = crc_tableil8_o56[(*(uint32_t *)p_buf) & 0x000000FF] ^
                crc_tableil8_o48[((*(uint32_t *)p_buf) >> 8) & 0x000000FF];

        term2 = (*(uint32_t *)p_buf) >> 16;
        crc = crc ^ term1 ^
                crc_tableil8_o40[term2  & 0x000000FF] ^
                crc_tableil8_o32[(term2 >> 8) & 0x000000FF];
        p_buf += 4;
}
{
        crc ^= *(uint32_t*) p_buf;
        p_buf += 4;
        uint32_t term1 = crc_tableil8_o88[crc & 0x000000FF] ^
                crc_tableil8_o80[(crc >> 8) & 0x000000FF];
        uint32_t term2 = crc >> 16;
        crc = term1 ^
              crc_tableil8_o72[term2 & 0x000000FF] ^
              crc_tableil8_o64[(term2 >> 8) & 0x000000FF];
        term1 = crc_tableil8_o56[(*(uint32_t *)p_buf) & 0x000000FF] ^
                crc_tableil8_o48[((*(uint32_t *)p_buf) >> 8) & 0x000000FF];

        term2 = (*(uint32_t *)p_buf) >> 16;
        crc = crc ^ term1 ^
                crc_tableil8_o40[term2  & 0x000000FF] ^
                crc_tableil8_o32[(term2 >> 8) & 0x000000FF];
        p_buf += 4;
}
{
        crc ^= *(uint32_t*) p_buf;
        p_buf += 4;
        uint32_t term1 = crc_tableil8_o88[crc & 0x000000FF] ^
                crc_tableil8_o80[(crc >> 8) & 0x000000FF];
        uint32_t term2 = crc >> 16;
        crc = term1 ^
              crc_tableil8_o72[term2 & 0x000000FF] ^
              crc_tableil8_o64[(term2 >> 8) & 0x000000FF];
        term1 = crc_tableil8_o56[(*(uint32_t *)p_buf) & 0x000000FF] ^
                crc_tableil8_o48[((*(uint32_t *)p_buf) >> 8) & 0x000000FF];

        term2 = (*(uint32_t *)p_buf) >> 16;
        crc = crc ^ term1 ^
                crc_tableil8_o40[term2  & 0x000000FF] ^
                crc_tableil8_o32[(term2 >> 8) & 0x000000FF];
        p_buf += 4;
}
{
        crc ^= *(uint32_t*) p_buf;
        p_buf += 4;
        uint32_t term1 = crc_tableil8_o88[crc & 0x000000FF] ^
                crc_tableil8_o80[(crc >> 8) & 0x000000FF];
        uint32_t term2 = crc >> 16;
        crc = term1 ^
              crc_tableil8_o72[term2 & 0x000000FF] ^
              crc_tableil8_o64[(term2 >> 8) & 0x000000FF];
        term1 = crc_tableil8_o56[(*(uint32_t *)p_buf) & 0x000000FF] ^
                crc_tableil8_o48[((*(uint32_t *)p_buf) >> 8) & 0x000000FF];

        term2 = (*(uint32_t *)p_buf) >> 16;
        crc = crc ^ term1 ^
                crc_tableil8_o40[term2  & 0x000000FF] ^
                crc_tableil8_o32[(term2 >> 8) & 0x000000FF];
        p_buf += 4;
}
    }

    for (size_t li=0; li < end_bytes; li++) {
        crc = crc_tableil8_o32[(crc ^ *p_buf++) & 0x000000FF] ^ (crc >> 8);
    }

    return crc;
}

// //////////////////////////////////////////////////////////
// constants

void init()
{
  // same algorithm as crc32_bitwise
  for (int i = 0; i <= 0xFF; i++)
  {
    uint32_t crc = i;
    for (int j = 0; j < 8; j++)
      crc = (crc >> 1) ^ ((crc & 1) * Polynomial);
    Crc32Lookup[0][i] = crc;
  }
  // ... and the following slicing-by-8 algorithm (from Intel):
  // http://www.intel.com/technology/comms/perfnet/download/CRC_generators.pdf
  // http://sourceforge.net/projects/slicing-by-8/
  for (int i = 0; i <= 0xFF; i++)
  {
    for (int j=0; j<15; j++)
      Crc32Lookup[j+1][i] = (Crc32Lookup[j][i] >> 8) ^ Crc32Lookup[0][Crc32Lookup[j][i] & 0xFF];
  }
}

// //////////////////////////////////////////////////////////
// test code


/// one gigabyte
const size_t NumBytes = 1024*1024*1024;
/// 4k chunks during last test
const size_t DefaultChunkSize = 4*1024;


#include <cstdio>
#include <ctime>
#ifdef _MSC_VER
#include <windows.h>
#endif

// timing
static double seconds()
{
#ifdef _MSC_VER
  LARGE_INTEGER frequency, now;
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter  (&now);
  return now.QuadPart / double(frequency.QuadPart);
#else
  return clock() / double(CLOCKS_PER_SEC);
#endif
}


int main(int, char**)
{
  printf("Please wait ...\n");
  init();

  // initialize
  char* data = new char[NumBytes];
  for (size_t i = 0; i < NumBytes; i++)
    data[i] = char(i & 0xFF);

  // re-use variables
  double startTime, duration;
  uint32_t crc;
/*
  // bitwise
  startTime = seconds();
  crc = crc32_bitwise(data, NumBytes);
  duration  = seconds() - startTime;
  printf("bitwise        : CRC=%08X, %.3fs, %.3f MB/s\n",
         crc, duration, (NumBytes / (1024*1024)) / duration);

  // half-byte
  startTime = seconds();
  crc = crc32_halfbyte(data, NumBytes);
  duration  = seconds() - startTime;
  printf("half-byte      : CRC=%08X, %.3fs, %.3f MB/s\n",
         crc, duration, (NumBytes / (1024*1024)) / duration);
*/
  // eight bytes at once
  startTime = seconds();
  crc = crc32_88bytes(data, NumBytes);
  duration  = seconds() - startTime;
  printf(" 88 bytes at once: CRC=%08X, %.3fs, %.3f MB/s\n",
         crc, duration, (NumBytes / (1024*1024)) / duration);

  // one byte at once
  startTime = seconds();
  crc = crc32_1byte(data, NumBytes);
  duration  = seconds() - startTime;
  printf("  1 byte  at once: CRC=%08X, %.3fs, %.3f MB/s\n",
         crc, duration, (NumBytes / (1024*1024)) / duration);

  // four bytes at once
  startTime = seconds();
  crc = crc32cSlicingBy4(data, NumBytes, 0);
  duration  = seconds() - startTime;
  printf("+  4 bytes at once: CRC=%08X, %.3fs, %.3f MB/s\n",
         crc, duration, (NumBytes / (1024*1024)) / duration);

  // four bytes at once
  startTime = seconds();
  crc = crc32cSlicingBy2x4(data, NumBytes, 0);
  duration  = seconds() - startTime;
  printf("+2*4 bytes at once: CRC=%08X, %.3fs, %.3f MB/s\n",
         crc, duration, (NumBytes / (1024*1024)) / duration);

  // four bytes at once
  startTime = seconds();
  crc = crc32cSlicingBy4x4(data, NumBytes, 0);
  duration  = seconds() - startTime;
  printf("+4*4 bytes at once: CRC=%08X, %.3fs, %.3f MB/s\n",
         crc, duration, (NumBytes / (1024*1024)) / duration);

  // four bytes at once
  startTime = seconds();
  crc = crc32cSlicingBy8(data, NumBytes, 0);
  duration  = seconds() - startTime;
  printf("+  8 bytes at once: CRC=%08X, %.3fs, %.3f MB/s\n",
         crc, duration, (NumBytes / (1024*1024)) / duration);

  // four bytes at once
  startTime = seconds();
  crc = crc32cSlicingBy16(data, NumBytes, 0);
  duration  = seconds() - startTime;
  printf("+2*8 bytes at once: CRC=%08X, %.3fs, %.3f MB/s\n",
         crc, duration, (NumBytes / (1024*1024)) / duration);

  // four bytes at once
  startTime = seconds();
  crc = crc32cSlicingBy32(data, NumBytes, 0);
  duration  = seconds() - startTime;
  printf("+4*8 bytes at once: CRC=%08X, %.3fs, %.3f MB/s\n",
         crc, duration, (NumBytes / (1024*1024)) / duration);

  // four bytes at once
  startTime = seconds();
  crc = crc32_4bytes(data, NumBytes);
  duration  = seconds() - startTime;
  printf("  4 bytes at once: CRC=%08X, %.3fs, %.3f MB/s\n",
         crc, duration, (NumBytes / (1024*1024)) / duration);

  // four bytes at once
  startTime = seconds();
  crc = crc32_2x4bytes(data, NumBytes);
  duration  = seconds() - startTime;
  printf("2*4 bytes at once: CRC=%08X, %.3fs, %.3f MB/s\n",
         crc, duration, (NumBytes / (1024*1024)) / duration);

  // four bytes at once
  startTime = seconds();
  crc = crc32_4x4bytes(data, NumBytes);
  duration  = seconds() - startTime;
  printf("4*4 bytes at once: CRC=%08X, %.3fs, %.3f MB/s\n",
         crc, duration, (NumBytes / (1024*1024)) / duration);

  // eight bytes at once
  startTime = seconds();
  crc = crc32_8bytes(data, NumBytes);
  duration  = seconds() - startTime;
  printf("  8 bytes at once: CRC=%08X, %.3fs, %.3f MB/s\n",
         crc, duration, (NumBytes / (1024*1024)) / duration);

  // eight bytes at once
  startTime = seconds();
  crc = crc32_16bytes(data, NumBytes);
  duration  = seconds() - startTime;
  printf(" 16 bytes at once: CRC=%08X, %.3fs, %.3f MB/s\n",
         crc, duration, (NumBytes / (1024*1024)) / duration);

  // eight bytes at once
  startTime = seconds();
  crc = crc32_2x16bytes(data, NumBytes);
  duration  = seconds() - startTime;
  printf("2*16 bytes at once: CRC=%08X, %.3fs, %.3f MB/s\n",
         crc, duration, (NumBytes / (1024*1024)) / duration);

  // eight bytes at once
  startTime = seconds();
  crc = crc32_2x8bytes(data, NumBytes);
  duration  = seconds() - startTime;
  printf("2*8 bytes at once: CRC=%08X, %.3fs, %.3f MB/s\n",
         crc, duration, (NumBytes / (1024*1024)) / duration);

  // eight bytes at once
  startTime = seconds();
  crc = crc32_4x8bytes(data, NumBytes);
  duration  = seconds() - startTime;
  printf("4*8 bytes at once: CRC=%08X, %.3fs, %.3f MB/s\n",
         crc, duration, (NumBytes / (1024*1024)) / duration);

  // eight bytes at once, process in 4k chunks
  startTime = seconds();
  crc = 0; // also default parameter of crc32_xx functions
  size_t bytesProcessed = 0;
  while (bytesProcessed < NumBytes)
  {
    size_t bytesLeft = NumBytes - bytesProcessed;
    size_t chunkSize = (DefaultChunkSize < bytesLeft) ? DefaultChunkSize : bytesLeft;

    crc = crc32_8bytes(data, chunkSize, crc);

    bytesProcessed += chunkSize;
  }
  duration  = seconds() - startTime;
  printf("chunked        : CRC=%08X, %.3fs, %.3f MB/s\n",
    crc, duration, (NumBytes / (1024*1024)) / duration);

  delete[] data;
  return 0;
}
