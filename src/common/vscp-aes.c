/*

This is an implementation of the AES128 algorithm, specifically ECB and CBC mode.

The implementation is verified against the test vectors in:
  National Institute of Standards and Technology Special Publication 800-38A 2001 ED

ECB-AES128
----------

  plain-text:
    6bc1bee22e409f96e93d7e117393172a
    ae2d8a571e03ac9c9eb76fac45af8e51
    30c81c46a35ce411e5fbc1191a0a52ef
    f69f2445df4f9b17ad2b417be66c3710

  key:
    2b7e151628aed2a6abf7158809cf4f3c

  resulting cipher
    3ad77bb40d7a3660a89ecaf32466ef97 
    f5d3d58503b9699de785895a96fdbaaf 
    43b1cd7f598ece23881b00e3ed030688 
    7b0c785e27e8ad3f8223207104725dd4 


NOTE:   String length must be evenly divisible by 16byte (str_len % 16 == 0)
        You should pad the end of the string with zeros if this is not the case.

*/




/*****************************************************************************/
/* Includes:                                                                 */
/*****************************************************************************/
#ifdef WIN32
#define _CRT_RAND_S  
#include <stdlib.h>
#endif

#include <stdint.h>
#include <stdio.h>  // file /dev/urandom
#include <string.h> // CBC mode, for memset
#include <stdlib.h> // malloc
#include "vscp-aes.h"

/*****************************************************************************/
/* Defines:                                                                  */
/*****************************************************************************/
// The number of columns comprising a state in AES. This is a constant in AES. Value=4
#define Nb          4u
#define BLOCKLEN    16u //Block length in bytes AES is 128b block only

/*#ifdef AES256
    #define Nk 8
    #define KEYLEN 32
    #define Nr 14
    #define keyExpSize 240
#elif defined(AES192)
    #define Nk 6
    #define KEYLEN 24
    #define Nr 12
    #define keyExpSize 208
#else
    #define Nk 4        // The number of 32 bit words in a key.
    #define KEYLEN 16   // Key length in bytes
    #define Nr 10       // The number of rounds in AES Cipher.
    #define keyExpSize 176
#endif*/

#define AES256_Nk           8
#define AES256_KEYLEN       32
#define AES256_Nr           14
#define AES256_keyExpSize   240

#define AES192_Nk           6
#define AES192_KEYLEN       24
#define AES192_Nr           12
#define AES192_keyExpSize   208

#define AES128_Nk           4
#define AES128_KEYLEN       16
#define AES128_Nr           10
#define AES128_keyExpSize   176

// jcallan@github points out that declaring Multiply as a function 
// reduces code size considerably with the Keil ARM compiler.
// See this link for more information: https://github.com/kokke/tiny-AES128-C/pull/3
#ifndef MULTIPLY_AS_A_FUNCTION
  #define MULTIPLY_AS_A_FUNCTION 0
#endif


/*****************************************************************************/
/* Private variables:                                                        */
/*****************************************************************************/
// state - array holding the intermediate results during decryption.
typedef uint8_t state_t[4][4];

typedef struct aes_state_t {
    state_t* state;

    // The array that stores the round keys.
    //uint8_t RoundKey[keyExpSize];
    uint8_t *RoundKey;

    // The Key input to the AES Program
    const uint8_t* Key;

#if defined(CBC) && CBC
    // Initial Vector used only for CBC mode
    uint8_t* Iv;
#endif
    
    // Keylength variables
    uint8_t type;       // AES128=0, AES192=1, AES256=2
    uint8_t Nk;         // The number of 32 bit words in a key.
    uint8_t KEYLEN;     // Key length in bytes
    uint8_t Nr;         // The number of rounds in AES Cipher.
    uint8_t keyExpSize;
    
} aes_state_t;




// The lookup-tables are marked const so they can be placed in read-only storage instead of RAM
// The numbers below can be computed dynamically trading ROM for RAM - 
// This can be useful in (embedded) bootloader applications, where ROM is often limited.
const uint8_t sbox[256] =   {
    //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

const uint8_t rsbox[256] =
{ 0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };

// The round constant word array, Rcon[i], contains the values given by 
// x to th e power (i-1) being powers of x (x is denoted as {02}) in the field GF(2^8)
static const uint8_t Rcon[256] = {
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
    0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
    0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
    0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
    0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
    0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
    0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
    0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
    0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
    0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
    0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
    0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
    0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
    0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
    0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
    0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d };


/*****************************************************************************/
/* Private functions:                                                        */
/*****************************************************************************/

static void init( uint8_t type, aes_state_t *state )
{
    switch ( type ) {
      
      case AES256:
        state->type = AES256;       
        state->Nk = AES256_Nk;         
        state->KEYLEN = AES256_KEYLEN;      
        state->Nr = AES256_Nr;         
        state->keyExpSize = AES256_keyExpSize;
        state->RoundKey = (uint8_t *)malloc( AES256_keyExpSize );
        break;
      
      case AES192:
        state->type = AES192;       
        state->Nk = AES192_Nk;         
        state->KEYLEN = AES192_KEYLEN;      
        state->Nr = AES192_Nr;         
        state->keyExpSize = AES192_keyExpSize;
        state->RoundKey = (uint8_t *)malloc( AES192_keyExpSize );
        break;
      
      case AES128:
        state->type = AES128;       
        state->Nk = AES128_Nk;         
        state->KEYLEN = AES128_KEYLEN;      
        state->Nr = AES128_Nr;         
        state->keyExpSize = AES128_keyExpSize;
        state->RoundKey = (uint8_t *)malloc( AES128_keyExpSize );
        break;
  }
}

static void cleanup( aes_state_t *state ) 
{
    free( state->RoundKey );
}

static uint8_t getSBoxValue(uint8_t num)
{
  return sbox[num];
}

static uint8_t getSBoxInvert(uint8_t num)
{
  return rsbox[num];
}

// This function produces Nb(Nr+1) round keys. The round keys are used in each round to decrypt the states. 
static void KeyExpansion( aes_state_t *state )
{
  uint32_t i, k;
  uint8_t tempa[4]; // Used for the column/row operations
  
  // The first round key is the key itself.
  for(i = 0; i < state->Nk; ++i)
  {
    state->RoundKey[(i * 4) + 0] = state->Key[(i * 4) + 0];
    state->RoundKey[(i * 4) + 1] = state->Key[(i * 4) + 1];
    state->RoundKey[(i * 4) + 2] = state->Key[(i * 4) + 2];
    state->RoundKey[(i * 4) + 3] = state->Key[(i * 4) + 3];
  }

  // All other round keys are found from the previous round keys.
  //i == Nk
  for(; i < Nb * (state->Nr + 1); ++i)
  {
    {
      tempa[0]=state->RoundKey[(i-1) * 4 + 0];
      tempa[1]=state->RoundKey[(i-1) * 4 + 1];
      tempa[2]=state->RoundKey[(i-1) * 4 + 2];
      tempa[3]=state->RoundKey[(i-1) * 4 + 3];
    }

    if (i % state->Nk == 0)
    {
      // This function shifts the 4 bytes in a word to the left once.
      // [a0,a1,a2,a3] becomes [a1,a2,a3,a0]

      // Function RotWord()
      {
        k = tempa[0];
        tempa[0] = tempa[1];
        tempa[1] = tempa[2];
        tempa[2] = tempa[3];
        tempa[3] = k;
      }

      // SubWord() is a function that takes a four-byte input word and 
      // applies the S-box to each of the four bytes to produce an output word.

      // Function Subword()
      {
        tempa[0] = getSBoxValue(tempa[0]);
        tempa[1] = getSBoxValue(tempa[1]);
        tempa[2] = getSBoxValue(tempa[2]);
        tempa[3] = getSBoxValue(tempa[3]);
      }

      tempa[0] =  tempa[0] ^ Rcon[i/state->Nk];
    }
if ( AES256 == state->type ) {
    if (i % state->Nk == 4)
    {
      // Function Subword()
      {
        tempa[0] = getSBoxValue(tempa[0]);
        tempa[1] = getSBoxValue(tempa[1]);
        tempa[2] = getSBoxValue(tempa[2]);
        tempa[3] = getSBoxValue(tempa[3]);
      }
    }
}
    state->RoundKey[i * 4 + 0] = state->RoundKey[(i - state->Nk) * 4 + 0] ^ tempa[0];
    state->RoundKey[i * 4 + 1] = state->RoundKey[(i - state->Nk) * 4 + 1] ^ tempa[1];
    state->RoundKey[i * 4 + 2] = state->RoundKey[(i - state->Nk) * 4 + 2] ^ tempa[2];
    state->RoundKey[i * 4 + 3] = state->RoundKey[(i - state->Nk) * 4 + 3] ^ tempa[3];
  }
}

// This function adds the round key to state.
// The round key is added to the state by an XOR function.
static void AddRoundKey( aes_state_t *state, uint8_t round )
{
  uint8_t i,j;
  for(i=0;i<4;++i)
  {
    for(j = 0; j < 4; ++j)
    {
      (*state->state)[i][j] ^= state->RoundKey[round * Nb * 4 + i * Nb + j];
    }
  }
}

// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
static void SubBytes( aes_state_t *state )
{
  uint8_t i, j;
  for(i = 0; i < 4; ++i)
  {
    for(j = 0; j < 4; ++j)
    {
      (*state->state)[j][i] = getSBoxValue((*state->state)[j][i]);
    }
  }
}

// The ShiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
static void ShiftRows( aes_state_t *state )
{
  uint8_t temp;

  // Rotate first row 1 columns to left  
  temp           = (*state->state)[0][1];
  (*state->state)[0][1] = (*state->state)[1][1];
  (*state->state)[1][1] = (*state->state)[2][1];
  (*state->state)[2][1] = (*state->state)[3][1];
  (*state->state)[3][1] = temp;

  // Rotate second row 2 columns to left  
  temp           = (*state->state)[0][2];
  (*state->state)[0][2] = (*state->state)[2][2];
  (*state->state)[2][2] = temp;

  temp       = (*state->state)[1][2];
  (*state->state)[1][2] = (*state->state)[3][2];
  (*state->state)[3][2] = temp;

  // Rotate third row 3 columns to left
  temp       = (*state->state)[0][3];
  (*state->state)[0][3] = (*state->state)[3][3];
  (*state->state)[3][3] = (*state->state)[2][3];
  (*state->state)[2][3] = (*state->state)[1][3];
  (*state->state)[1][3] = temp;
}

static uint8_t xtime(uint8_t x)
{
  return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
}

// MixColumns function mixes the columns of the state matrix
static void MixColumns( aes_state_t *state )
{
  uint8_t i;
  uint8_t Tmp,Tm,t;
  for(i = 0; i < 4; ++i)
  {  
    t   = (*state->state)[i][0];
    Tmp = (*state->state)[i][0] ^ (*state->state)[i][1] ^ (*state->state)[i][2] ^ (*state->state)[i][3] ;
    Tm  = (*state->state)[i][0] ^ (*state->state)[i][1] ; Tm = xtime(Tm);  (*state->state)[i][0] ^= Tm ^ Tmp ;
    Tm  = (*state->state)[i][1] ^ (*state->state)[i][2] ; Tm = xtime(Tm);  (*state->state)[i][1] ^= Tm ^ Tmp ;
    Tm  = (*state->state)[i][2] ^ (*state->state)[i][3] ; Tm = xtime(Tm);  (*state->state)[i][2] ^= Tm ^ Tmp ;
    Tm  = (*state->state)[i][3] ^ t ;        Tm = xtime(Tm);  (*state->state)[i][3] ^= Tm ^ Tmp ;
  }
}

// Multiply is used to multiply numbers in the field GF(2^8)
#if MULTIPLY_AS_A_FUNCTION
static uint8_t Multiply(uint8_t x, uint8_t y)
{
  return (((y & 1) * x) ^
       ((y>>1 & 1) * xtime(x)) ^
       ((y>>2 & 1) * xtime(xtime(x))) ^
       ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^
       ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))));
  }
#else
#define Multiply(x, y)                                \
      (  ((y & 1) * x) ^                              \
      ((y>>1 & 1) * xtime(x)) ^                       \
      ((y>>2 & 1) * xtime(xtime(x))) ^                \
      ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^         \
      ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))))   \

#endif

// MixColumns function mixes the columns of the state matrix.
// The method used to multiply may be difficult to understand for the inexperienced.
// Please use the references to gain more information.
static void InvMixColumns( aes_state_t *state )
{
  int i;
  uint8_t a,b,c,d;
  for(i=0;i<4;++i)
  { 
    a = (*state->state)[i][0];
    b = (*state->state)[i][1];
    c = (*state->state)[i][2];
    d = (*state->state)[i][3];

    (*state->state)[i][0] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
    (*state->state)[i][1] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
    (*state->state)[i][2] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
    (*state->state)[i][3] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
  }
}


// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
static void InvSubBytes( aes_state_t *state )
{
  uint8_t i,j;
  for(i=0;i<4;++i)
  {
    for(j=0;j<4;++j)
    {
      (*state->state)[j][i] = getSBoxInvert((*state->state)[j][i]);
    }
  }
}

static void InvShiftRows( aes_state_t *state )
{
  uint8_t temp;

  // Rotate first row 1 columns to right  
  temp=(*state->state)[3][1];
  (*state->state)[3][1]=(*state->state)[2][1];
  (*state->state)[2][1]=(*state->state)[1][1];
  (*state->state)[1][1]=(*state->state)[0][1];
  (*state->state)[0][1]=temp;

  // Rotate second row 2 columns to right 
  temp=(*state->state)[0][2];
  (*state->state)[0][2]=(*state->state)[2][2];
  (*state->state)[2][2]=temp;

  temp=(*state->state)[1][2];
  (*state->state)[1][2]=(*state->state)[3][2];
  (*state->state)[3][2]=temp;

  // Rotate third row 3 columns to right
  temp=(*state->state)[0][3];
  (*state->state)[0][3]=(*state->state)[1][3];
  (*state->state)[1][3]=(*state->state)[2][3];
  (*state->state)[2][3]=(*state->state)[3][3];
  (*state->state)[3][3]=temp;
}


// Cipher is the main function that encrypts the PlainText.
static void Cipher( aes_state_t *state )
{
  uint8_t round = 0;

  // Add the First round key to the state before starting the rounds.
  AddRoundKey(state,0); 
  
  // There will be Nr rounds.
  // The first Nr-1 rounds are identical.
  // These Nr-1 rounds are executed in the loop below.
  for(round = 1; round < state->Nr; ++round)
  {
    SubBytes(state);
    ShiftRows(state);
    MixColumns(state);
    AddRoundKey(state,round);
  }
  
  // The last round is given below.
  // The MixColumns function is not here in the last round.
  SubBytes(state);
  ShiftRows(state);
  AddRoundKey(state,state->Nr);
}

static void InvCipher( aes_state_t *state )
{
  uint8_t round=0;

  // Add the First round key to the state before starting the rounds.
  AddRoundKey(state, state->Nr); 

  // There will be Nr rounds.
  // The first Nr-1 rounds are identical.
  // These Nr-1 rounds are executed in the loop below.
  for(round=state->Nr-1;round>0;round--)
  {
    InvShiftRows(state);
    InvSubBytes(state);
    AddRoundKey(state,round);
    InvMixColumns(state);
  }
  
  // The last round is given below.
  // The MixColumns function is not here in the last round.
  InvShiftRows(state);
  InvSubBytes(state);
  AddRoundKey(state,0);
}


/*****************************************************************************/
/* Public functions:                                                         */
/*****************************************************************************/
#if defined(ECB) && ECB


void AES_ECB_encrypt(uint8_t type,const uint8_t* input, const uint8_t* key, uint8_t* output, const uint32_t length)
{
  aes_state_t state;  
  
  init( type, &state ); // Init. cypher parameters
    
  // Copy input to output, and work in-memory on output
  memcpy(output, input, length);
  state.state = (state_t*)output;

  state.Key = key;
  KeyExpansion( &state );

  // The next function call encrypts the PlainText with the Key using AES algorithm.
  Cipher( &state );
  
  cleanup(&state);
  
}

void AES_ECB_decrypt(uint8_t type,const uint8_t* input, const uint8_t* key, uint8_t *output, const uint32_t length)
{
  aes_state_t state;
  
  init( type, &state ); // Init. cypher parameters
  
  // Copy input to output, and work in-memory on output
  memcpy(output, input, length);
  state.state = (state_t*)output;

  // The KeyExpansion routine must be called before encryption.
  state.Key = key;
  KeyExpansion(&state);

  InvCipher(&state);
  
  cleanup(&state);
  
}


#endif // #if defined(ECB) && ECB





#if defined(CBC) && CBC


static void XorWithIv(aes_state_t *state, uint8_t* buf)
{
  uint8_t i;
  for(i = 0; i < BLOCKLEN; ++i) //WAS for(i = 0; i < KEYLEN; ++i) but the block in AES is always 128bit so 16 bytes!
  {
    buf[i] ^= state->Iv[i];
  }
}

void AES_CBC_encrypt_buffer(uint8_t type,uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv)
{
  uintptr_t i;
  uint8_t extra = length % BLOCKLEN; /* Remaining bytes in the last non-full block */
  aes_state_t state;
  
  init( type, &state ); // Init. cypher parameters

  memcpy(output, input, BLOCKLEN);
  state.state = (state_t*)output;

  // Skip the key expansion if key is passed as 0
  if(0 != key)
  {
    state.Key = key;
    KeyExpansion(&state);
  }

  if(iv != 0)
  {
    state.Iv = (uint8_t*)iv;
  }

  for(i = 0; i < length; i += BLOCKLEN)
  {
    XorWithIv(&state, input);
    memcpy(output, input, BLOCKLEN);
    state.state = (state_t*)output;
    Cipher(&state);
    state.Iv = output;
    input += BLOCKLEN;
    output += BLOCKLEN;
    //printf("Step %d - %d", i/16, i);
  }

  if(extra)
  {
    memcpy(output, input, extra);
    state.state = (state_t*)output;
    Cipher(&state);
  }
  
  cleanup(&state);
  
}

void AES_CBC_decrypt_buffer(uint8_t type,uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv)
{
  uintptr_t i;
  uint8_t extra = length % BLOCKLEN; /* Remaining bytes in the last non-full block */
  aes_state_t state;
  
  init( type, &state ); // Init. cypher parameters

  memcpy(output, input, BLOCKLEN);
  state.state = (state_t*)output;
  
  // Skip the key expansion if key is passed as 0
  if(0 != key)
  {
    state.Key = key;
    KeyExpansion(&state);
  }

  // If iv is passed as 0, we continue to encrypt without re-setting the Iv
  if(iv != 0)
  {
    state.Iv = (uint8_t*)iv;
  }

  for(i = 0; i < length; i += BLOCKLEN)
  {
    memcpy(output, input, BLOCKLEN);
    state.state = (state_t*)output;
    InvCipher(&state);
    XorWithIv(&state,output);
    state.Iv = input;
    input += BLOCKLEN;
    output += BLOCKLEN;
  }

  if(extra)
  {
    memcpy(output, input, extra);
    state.state = (state_t*)output;
    InvCipher(&state);
  }
  
  cleanup(&state);
  
}

size_t getRandomIV( uint8_t *buf, size_t len )
{
#ifdef WIN32
    int random;
    
    for ( int i=0; i<len; i++ ) {
        if ( !rand_s( &random ) ) {
            buf[i] = (uint8_t)random;
        }
        else {
            buf[i] = i;
        }
    }

    return len;
#else    
    FILE *fp;
    fp = fopen("/dev/urandom", "r");
    if (NULL == fp) return 0;
    size_t nread = fread( buf, 1, len, fp);
    fclose(fp);
    return nread;
#endif    
    
}

#endif // #if defined(CBC) && CBC


