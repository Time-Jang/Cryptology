/*  ============================================================================================================ *

                                    주 의 사 항


    1. 주석으로 표현된 구현 블록 0에서 12번까지 구현하여 128비트 AES 암호화 알고리즘을 구현해야 함
    2. AES128(...) 함수의 호출과 리턴이 여러번 반복되더라도 메모리 누수가 생기지 않게 함
    3. AddRoundKey 함수를 구현할 때에도 파라미터 rKey는 사전에 선언된 지역 배열을 가리키도록 해야 함
       (정확한 구현을 위해서는 포인터 개념의 이해가 필요함)
    4. 배열의 인덱스 계산시 아래에 정의된 KEY_SIZE, ROUNDKEY_SIZE, BLOCK_SIZE를 이용해야 함
       (상수 그대로 사용하면 안됨. 예로, 4, 16는 안되고 KEY_SIZE/4, BLOCK_SIZE로 사용해야 함)

 *  ============================================================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include "AES128.h"

#define KEY_SIZE 16
#define ROUNDKEY_SIZE 176
#define BLOCK_SIZE 16


/*********************************************** { 구현 0 시작 } ********************************************/
BYTE S_BOX[BLOCK_SIZE*BLOCK_SIZE] =
{
	0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
	0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
	0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
	0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
	0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
	0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
	0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
	0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
	0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
	0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
	0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
	0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
	0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
	0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
	0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
	0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

BYTE RS_BOX[BLOCK_SIZE*BLOCK_SIZE] =
{
	0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
	0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
	0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
	0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
	0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
	0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
	0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
	0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
	0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
	0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
	0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
	0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
	0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
	0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
	0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
	0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

BYTE MCOL[BLOCK_SIZE] =
{
	2, 3, 1, 1,
	1, 2, 3, 1,
	1, 1, 2, 3,
	3, 1, 1, 2
};

BYTE RMCOL[BLOCK_SIZE] =
{
	14, 11, 13, 9,
	9, 14, 11, 13,
	13, 9, 14, 11,
	11, 13, 9, 14
};

BYTE RCON[] = 
{
	0x01, 0x00, 0x00, 0x00,
	0x02, 0x00, 0x00, 0x00,
	0x04, 0x00, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00,
	0x10, 0x00, 0x00, 0x00,
	0x20, 0x00, 0x00, 0x00,
	0x40, 0x00, 0x00, 0x00,
	0x80, 0x00, 0x00, 0x00,
	0x1B, 0x00, 0x00, 0x00,
	0x36, 0x00, 0x00, 0x00
};

BYTE mul1[256] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 
	0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 
	0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 
	0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 
	0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 
	0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 
	0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

BYTE mul2[] = 
{
	0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x16,0x18,0x1a,0x1c,0x1e,
	0x20,0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,0x30,0x32,0x34,0x36,0x38,0x3a,0x3c,0x3e,
	0x40,0x42,0x44,0x46,0x48,0x4a,0x4c,0x4e,0x50,0x52,0x54,0x56,0x58,0x5a,0x5c,0x5e,
	0x60,0x62,0x64,0x66,0x68,0x6a,0x6c,0x6e,0x70,0x72,0x74,0x76,0x78,0x7a,0x7c,0x7e,
	0x80,0x82,0x84,0x86,0x88,0x8a,0x8c,0x8e,0x90,0x92,0x94,0x96,0x98,0x9a,0x9c,0x9e,
	0xa0,0xa2,0xa4,0xa6,0xa8,0xaa,0xac,0xae,0xb0,0xb2,0xb4,0xb6,0xb8,0xba,0xbc,0xbe,
	0xc0,0xc2,0xc4,0xc6,0xc8,0xca,0xcc,0xce,0xd0,0xd2,0xd4,0xd6,0xd8,0xda,0xdc,0xde,
	0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xee,0xf0,0xf2,0xf4,0xf6,0xf8,0xfa,0xfc,0xfe,
	0x1b,0x19,0x1f,0x1d,0x13,0x11,0x17,0x15,0x0b,0x09,0x0f,0x0d,0x03,0x01,0x07,0x05,
	0x3b,0x39,0x3f,0x3d,0x33,0x31,0x37,0x35,0x2b,0x29,0x2f,0x2d,0x23,0x21,0x27,0x25,
	0x5b,0x59,0x5f,0x5d,0x53,0x51,0x57,0x55,0x4b,0x49,0x4f,0x4d,0x43,0x41,0x47,0x45,
	0x7b,0x79,0x7f,0x7d,0x73,0x71,0x77,0x75,0x6b,0x69,0x6f,0x6d,0x63,0x61,0x67,0x65,
	0x9b,0x99,0x9f,0x9d,0x93,0x91,0x97,0x95,0x8b,0x89,0x8f,0x8d,0x83,0x81,0x87,0x85,
	0xbb,0xb9,0xbf,0xbd,0xb3,0xb1,0xb7,0xb5,0xab,0xa9,0xaf,0xad,0xa3,0xa1,0xa7,0xa5,
	0xdb,0xd9,0xdf,0xdd,0xd3,0xd1,0xd7,0xd5,0xcb,0xc9,0xcf,0xcd,0xc3,0xc1,0xc7,0xc5,
	0xfb,0xf9,0xff,0xfd,0xf3,0xf1,0xf7,0xf5,0xeb,0xe9,0xef,0xed,0xe3,0xe1,0xe7,0xe5
};

BYTE mul3[] = 
{
	0x00,0x03,0x06,0x05,0x0c,0x0f,0x0a,0x09,0x18,0x1b,0x1e,0x1d,0x14,0x17,0x12,0x11,
	0x30,0x33,0x36,0x35,0x3c,0x3f,0x3a,0x39,0x28,0x2b,0x2e,0x2d,0x24,0x27,0x22,0x21,
	0x60,0x63,0x66,0x65,0x6c,0x6f,0x6a,0x69,0x78,0x7b,0x7e,0x7d,0x74,0x77,0x72,0x71,
	0x50,0x53,0x56,0x55,0x5c,0x5f,0x5a,0x59,0x48,0x4b,0x4e,0x4d,0x44,0x47,0x42,0x41,
	0xc0,0xc3,0xc6,0xc5,0xcc,0xcf,0xca,0xc9,0xd8,0xdb,0xde,0xdd,0xd4,0xd7,0xd2,0xd1,
	0xf0,0xf3,0xf6,0xf5,0xfc,0xff,0xfa,0xf9,0xe8,0xeb,0xee,0xed,0xe4,0xe7,0xe2,0xe1,
	0xa0,0xa3,0xa6,0xa5,0xac,0xaf,0xaa,0xa9,0xb8,0xbb,0xbe,0xbd,0xb4,0xb7,0xb2,0xb1,
	0x90,0x93,0x96,0x95,0x9c,0x9f,0x9a,0x99,0x88,0x8b,0x8e,0x8d,0x84,0x87,0x82,0x81,
	0x9b,0x98,0x9d,0x9e,0x97,0x94,0x91,0x92,0x83,0x80,0x85,0x86,0x8f,0x8c,0x89,0x8a,
	0xab,0xa8,0xad,0xae,0xa7,0xa4,0xa1,0xa2,0xb3,0xb0,0xb5,0xb6,0xbf,0xbc,0xb9,0xba,
	0xfb,0xf8,0xfd,0xfe,0xf7,0xf4,0xf1,0xf2,0xe3,0xe0,0xe5,0xe6,0xef,0xec,0xe9,0xea,
	0xcb,0xc8,0xcd,0xce,0xc7,0xc4,0xc1,0xc2,0xd3,0xd0,0xd5,0xd6,0xdf,0xdc,0xd9,0xda,
	0x5b,0x58,0x5d,0x5e,0x57,0x54,0x51,0x52,0x43,0x40,0x45,0x46,0x4f,0x4c,0x49,0x4a,
	0x6b,0x68,0x6d,0x6e,0x67,0x64,0x61,0x62,0x73,0x70,0x75,0x76,0x7f,0x7c,0x79,0x7a,
	0x3b,0x38,0x3d,0x3e,0x37,0x34,0x31,0x32,0x23,0x20,0x25,0x26,0x2f,0x2c,0x29,0x2a,
	0x0b,0x08,0x0d,0x0e,0x07,0x04,0x01,0x02,0x13,0x10,0x15,0x16,0x1f,0x1c,0x19,0x1a
};

BYTE mul9[] = 
{
	0x00,0x09,0x12,0x1b,0x24,0x2d,0x36,0x3f,0x48,0x41,0x5a,0x53,0x6c,0x65,0x7e,0x77,
	0x90,0x99,0x82,0x8b,0xb4,0xbd,0xa6,0xaf,0xd8,0xd1,0xca,0xc3,0xfc,0xf5,0xee,0xe7,
	0x3b,0x32,0x29,0x20,0x1f,0x16,0x0d,0x04,0x73,0x7a,0x61,0x68,0x57,0x5e,0x45,0x4c,
	0xab,0xa2,0xb9,0xb0,0x8f,0x86,0x9d,0x94,0xe3,0xea,0xf1,0xf8,0xc7,0xce,0xd5,0xdc,
	0x76,0x7f,0x64,0x6d,0x52,0x5b,0x40,0x49,0x3e,0x37,0x2c,0x25,0x1a,0x13,0x08,0x01,
	0xe6,0xef,0xf4,0xfd,0xc2,0xcb,0xd0,0xd9,0xae,0xa7,0xbc,0xb5,0x8a,0x83,0x98,0x91,
	0x4d,0x44,0x5f,0x56,0x69,0x60,0x7b,0x72,0x05,0x0c,0x17,0x1e,0x21,0x28,0x33,0x3a,
	0xdd,0xd4,0xcf,0xc6,0xf9,0xf0,0xeb,0xe2,0x95,0x9c,0x87,0x8e,0xb1,0xb8,0xa3,0xaa,
	0xec,0xe5,0xfe,0xf7,0xc8,0xc1,0xda,0xd3,0xa4,0xad,0xb6,0xbf,0x80,0x89,0x92,0x9b,
	0x7c,0x75,0x6e,0x67,0x58,0x51,0x4a,0x43,0x34,0x3d,0x26,0x2f,0x10,0x19,0x02,0x0b,
	0xd7,0xde,0xc5,0xcc,0xf3,0xfa,0xe1,0xe8,0x9f,0x96,0x8d,0x84,0xbb,0xb2,0xa9,0xa0,
	0x47,0x4e,0x55,0x5c,0x63,0x6a,0x71,0x78,0x0f,0x06,0x1d,0x14,0x2b,0x22,0x39,0x30,
	0x9a,0x93,0x88,0x81,0xbe,0xb7,0xac,0xa5,0xd2,0xdb,0xc0,0xc9,0xf6,0xff,0xe4,0xed,
	0x0a,0x03,0x18,0x11,0x2e,0x27,0x3c,0x35,0x42,0x4b,0x50,0x59,0x66,0x6f,0x74,0x7d,
	0xa1,0xa8,0xb3,0xba,0x85,0x8c,0x97,0x9e,0xe9,0xe0,0xfb,0xf2,0xcd,0xc4,0xdf,0xd6,
	0x31,0x38,0x23,0x2a,0x15,0x1c,0x07,0x0e,0x79,0x70,0x6b,0x62,0x5d,0x54,0x4f,0x46
};

BYTE mul11[] =
{	
	0x00,0x0b,0x16,0x1d,0x2c,0x27,0x3a,0x31,0x58,0x53,0x4e,0x45,0x74,0x7f,0x62,0x69,
	0xb0,0xbb,0xa6,0xad,0x9c,0x97,0x8a,0x81,0xe8,0xe3,0xfe,0xf5,0xc4,0xcf,0xd2,0xd9,
	0x7b,0x70,0x6d,0x66,0x57,0x5c,0x41,0x4a,0x23,0x28,0x35,0x3e,0x0f,0x04,0x19,0x12,
	0xcb,0xc0,0xdd,0xd6,0xe7,0xec,0xf1,0xfa,0x93,0x98,0x85,0x8e,0xbf,0xb4,0xa9,0xa2,
	0xf6,0xfd,0xe0,0xeb,0xda,0xd1,0xcc,0xc7,0xae,0xa5,0xb8,0xb3,0x82,0x89,0x94,0x9f,
	0x46,0x4d,0x50,0x5b,0x6a,0x61,0x7c,0x77,0x1e,0x15,0x08,0x03,0x32,0x39,0x24,0x2f,
	0x8d,0x86,0x9b,0x90,0xa1,0xaa,0xb7,0xbc,0xd5,0xde,0xc3,0xc8,0xf9,0xf2,0xef,0xe4,
	0x3d,0x36,0x2b,0x20,0x11,0x1a,0x07,0x0c,0x65,0x6e,0x73,0x78,0x49,0x42,0x5f,0x54,
	0xf7,0xfc,0xe1,0xea,0xdb,0xd0,0xcd,0xc6,0xaf,0xa4,0xb9,0xb2,0x83,0x88,0x95,0x9e,
	0x47,0x4c,0x51,0x5a,0x6b,0x60,0x7d,0x76,0x1f,0x14,0x09,0x02,0x33,0x38,0x25,0x2e,
	0x8c,0x87,0x9a,0x91,0xa0,0xab,0xb6,0xbd,0xd4,0xdf,0xc2,0xc9,0xf8,0xf3,0xee,0xe5,
	0x3c,0x37,0x2a,0x21,0x10,0x1b,0x06,0x0d,0x64,0x6f,0x72,0x79,0x48,0x43,0x5e,0x55,
	0x01,0x0a,0x17,0x1c,0x2d,0x26,0x3b,0x30,0x59,0x52,0x4f,0x44,0x75,0x7e,0x63,0x68,
	0xb1,0xba,0xa7,0xac,0x9d,0x96,0x8b,0x80,0xe9,0xe2,0xff,0xf4,0xc5,0xce,0xd3,0xd8,
	0x7a,0x71,0x6c,0x67,0x56,0x5d,0x40,0x4b,0x22,0x29,0x34,0x3f,0x0e,0x05,0x18,0x13,
	0xca,0xc1,0xdc,0xd7,0xe6,0xed,0xf0,0xfb,0x92,0x99,0x84,0x8f,0xbe,0xb5,0xa8,0xa3
};

BYTE mul13[] =
{
	0x00,0x0d,0x1a,0x17,0x34,0x39,0x2e,0x23,0x68,0x65,0x72,0x7f,0x5c,0x51,0x46,0x4b,
	0xd0,0xdd,0xca,0xc7,0xe4,0xe9,0xfe,0xf3,0xb8,0xb5,0xa2,0xaf,0x8c,0x81,0x96,0x9b,
	0xbb,0xb6,0xa1,0xac,0x8f,0x82,0x95,0x98,0xd3,0xde,0xc9,0xc4,0xe7,0xea,0xfd,0xf0,
	0x6b,0x66,0x71,0x7c,0x5f,0x52,0x45,0x48,0x03,0x0e,0x19,0x14,0x37,0x3a,0x2d,0x20,
	0x6d,0x60,0x77,0x7a,0x59,0x54,0x43,0x4e,0x05,0x08,0x1f,0x12,0x31,0x3c,0x2b,0x26,
	0xbd,0xb0,0xa7,0xaa,0x89,0x84,0x93,0x9e,0xd5,0xd8,0xcf,0xc2,0xe1,0xec,0xfb,0xf6,
	0xd6,0xdb,0xcc,0xc1,0xe2,0xef,0xf8,0xf5,0xbe,0xb3,0xa4,0xa9,0x8a,0x87,0x90,0x9d,
	0x06,0x0b,0x1c,0x11,0x32,0x3f,0x28,0x25,0x6e,0x63,0x74,0x79,0x5a,0x57,0x40,0x4d,
	0xda,0xd7,0xc0,0xcd,0xee,0xe3,0xf4,0xf9,0xb2,0xbf,0xa8,0xa5,0x86,0x8b,0x9c,0x91,
	0x0a,0x07,0x10,0x1d,0x3e,0x33,0x24,0x29,0x62,0x6f,0x78,0x75,0x56,0x5b,0x4c,0x41,
	0x61,0x6c,0x7b,0x76,0x55,0x58,0x4f,0x42,0x09,0x04,0x13,0x1e,0x3d,0x30,0x27,0x2a,
	0xb1,0xbc,0xab,0xa6,0x85,0x88,0x9f,0x92,0xd9,0xd4,0xc3,0xce,0xed,0xe0,0xf7,0xfa,
	0xb7,0xba,0xad,0xa0,0x83,0x8e,0x99,0x94,0xdf,0xd2,0xc5,0xc8,0xeb,0xe6,0xf1,0xfc,
	0x67,0x6a,0x7d,0x70,0x53,0x5e,0x49,0x44,0x0f,0x02,0x15,0x18,0x3b,0x36,0x21,0x2c,
	0x0c,0x01,0x16,0x1b,0x38,0x35,0x22,0x2f,0x64,0x69,0x7e,0x73,0x50,0x5d,0x4a,0x47,
	0xdc,0xd1,0xc6,0xcb,0xe8,0xe5,0xf2,0xff,0xb4,0xb9,0xae,0xa3,0x80,0x8d,0x9a,0x97
};

BYTE mul14[] =
{
	0x00,0x0e,0x1c,0x12,0x38,0x36,0x24,0x2a,0x70,0x7e,0x6c,0x62,0x48,0x46,0x54,0x5a,
	0xe0,0xee,0xfc,0xf2,0xd8,0xd6,0xc4,0xca,0x90,0x9e,0x8c,0x82,0xa8,0xa6,0xb4,0xba,
	0xdb,0xd5,0xc7,0xc9,0xe3,0xed,0xff,0xf1,0xab,0xa5,0xb7,0xb9,0x93,0x9d,0x8f,0x81,
	0x3b,0x35,0x27,0x29,0x03,0x0d,0x1f,0x11,0x4b,0x45,0x57,0x59,0x73,0x7d,0x6f,0x61,
	0xad,0xa3,0xb1,0xbf,0x95,0x9b,0x89,0x87,0xdd,0xd3,0xc1,0xcf,0xe5,0xeb,0xf9,0xf7,
	0x4d,0x43,0x51,0x5f,0x75,0x7b,0x69,0x67,0x3d,0x33,0x21,0x2f,0x05,0x0b,0x19,0x17,
	0x76,0x78,0x6a,0x64,0x4e,0x40,0x52,0x5c,0x06,0x08,0x1a,0x14,0x3e,0x30,0x22,0x2c,
	0x96,0x98,0x8a,0x84,0xae,0xa0,0xb2,0xbc,0xe6,0xe8,0xfa,0xf4,0xde,0xd0,0xc2,0xcc,
	0x41,0x4f,0x5d,0x53,0x79,0x77,0x65,0x6b,0x31,0x3f,0x2d,0x23,0x09,0x07,0x15,0x1b,
	0xa1,0xaf,0xbd,0xb3,0x99,0x97,0x85,0x8b,0xd1,0xdf,0xcd,0xc3,0xe9,0xe7,0xf5,0xfb,
	0x9a,0x94,0x86,0x88,0xa2,0xac,0xbe,0xb0,0xea,0xe4,0xf6,0xf8,0xd2,0xdc,0xce,0xc0,
	0x7a,0x74,0x66,0x68,0x42,0x4c,0x5e,0x50,0x0a,0x04,0x16,0x18,0x32,0x3c,0x2e,0x20,
	0xec,0xe2,0xf0,0xfe,0xd4,0xda,0xc8,0xc6,0x9c,0x92,0x80,0x8e,0xa4,0xaa,0xb8,0xb6,
	0x0c,0x02,0x10,0x1e,0x34,0x3a,0x28,0x26,0x7c,0x72,0x60,0x6e,0x44,0x4a,0x58,0x56,
	0x37,0x39,0x2b,0x25,0x0f,0x01,0x13,0x1d,0x47,0x49,0x5b,0x55,0x7f,0x71,0x63,0x6d,
	0xd7,0xd9,0xcb,0xc5,0xef,0xe1,0xf3,0xfd,0xa7,0xa9,0xbb,0xb5,0x9f,0x91,0x83,0x8d
};

BYTE *mul[] = {
	mul1, mul2, mul3, NULL, NULL, NULL, NULL, NULL, mul9, NULL, mul11, NULL, mul13, mul14
};
/*********************************************** { 구현 0 종료 } ********************************************/


/*  <키스케줄링 함수>
 *   
 *  key         키스케줄링을 수행할 16바이트 키
 *  roundKey    키스케줄링의 결과인 176바이트 라운드키가 담길 공간 , expandkey의 결과
 */
void expandKey(BYTE *key, BYTE *roundKey){   //                         검산

    /*********************************************** { 구현 1 시작 } ********************************************/
	int i,j,k;
	BYTE temp;
	BYTE g[KEY_SIZE/4]; // 해당 라운드에서 사용되는 g
	BYTE tempkey[BLOCK_SIZE]; //해당 라운드에서 사용되는 key
	for(k=0;k<BLOCK_SIZE;k++) //첫 roundkey에 key 삽입
		roundKey[k]=key[k];
	for(i=0;i < ((ROUNDKEY_SIZE/BLOCK_SIZE)-1);i++) //나머지 roundkey
	{
		for(k = 0; k < BLOCK_SIZE; k++) //계산할 word들을 tempkey에 담기
			tempkey[k]=roundKey[(BLOCK_SIZE) * i + k];
		// G 구하기 for문 하나로 줄여봄
		for(j = BLOCK_SIZE - 4; j < BLOCK_SIZE; j++) //g에 word 3복사
			g[j - (BLOCK_SIZE -4)] = tempkey[j];
		
		for(j = 0; j < (KEY_SIZE / 4); j++) //Left Rotation Word
		{
			temp = g[j];
			g[j] = g[j + 1];
			g[j + 1] = temp;
		}
		
		for(j = 0; j < (KEY_SIZE / 4); j++) //SubWord
			g[j] = S_BOX[g[j]];
		
		for(j = 0; j < (KEY_SIZE / 4); j++) //XOR with RCON
			g[j] = g[j] ^ RCON[4 * i + j];
		//G 완성
		
		for(j=0; j < (KEY_SIZE/4); j++) //첫번째 word와 g를 연산해서 roundKey에 삽입
			roundKey[(BLOCK_SIZE * (i + 1)) + j] = tempkey[j] ^ g[j];
		for(j = 1; j < (KEY_SIZE/4); j++) // 나머지 word 게산 후 넣기
			for(k = 0; k < (KEY_SIZE/4); k++)
				roundKey[(BLOCK_SIZE * (i+1)) + ((KEY_SIZE/4) * j) + k ] = tempkey[(j * 4) + k] ^ roundKey[BLOCK_SIZE + (BLOCK_SIZE * i) + ((KEY_SIZE/4) * (j - 1)) + k];
	}



    /*********************************************** { 구현 1 종료 } ********************************************/

}




/*  <SubBytes 함수>
 *   
 *  block   SubBytes 수행할 16바이트 블록. 수행 결과는 해당 배열에 바로 반영
 *  mode    SubBytes 수행 모드
 */
 BYTE* subBytes(BYTE *block, int mode){
	 int i;
    /* 필요하다 생각하면 추가 선언 */

    switch(mode){
		
        case ENC:
            /*********************************************** { 구현 2 시작 } ********************************************/
			for(i=0;i<BLOCK_SIZE;i++)
				block[i] =  S_BOX[block[i]];

            /*********************************************** { 구현 2 종료 } ********************************************/            
            break;

        case DEC:
            /*********************************************** { 구현 3 시작 } ********************************************/
			for(i=0;i<BLOCK_SIZE;i++)
				block[i] = RS_BOX[block[i]];


            /*********************************************** { 구현 3 종료 } ********************************************/
            break;

        default:
            fprintf(stderr, "Invalid mode!\n");
            exit(1);
    }
    
    return block;
}


/*  <ShiftRows 함수>
 *   
 *  block   ShiftRows 수행할 16바이트 블록. 수행 결과는 해당 배열에 바로 반영
 *  mode    ShiftRows 수행 모드
 */
BYTE* shiftRows(BYTE *block, int mode){ 
	int i;
	BYTE temp;
    /* 필요하다 생각하면 추가 선언 */   

    switch(mode){

        case ENC:
            /*********************************************** { 구현 4 시작 } ********************************************/
			for(i = 0; i < (KEY_SIZE/4) - 1; i++)// row 2
			{
				temp = block[1 + ((KEY_SIZE/4) * i)];
				block[1 + ((KEY_SIZE/4) * i)] = block[1 + ((KEY_SIZE/4) * (i + 1))];
				block[1 + ((KEY_SIZE/4) * (i+1))] = temp;
			}
			for(i = 0; i < ((KEY_SIZE/4)/2); i++) // row 3
			{
				temp = block[2 + (i * (KEY_SIZE/4))];
				block[2 + (i * (KEY_SIZE/4))] = block[2 + ((KEY_SIZE/4) * (i + 2))];
				block[2 + ((KEY_SIZE/4) * (i + 2))] = temp;
			}
			for(i = (KEY_SIZE/4) - 1; i > 0; i--) //row4
			{
				temp = block[3 + ((KEY_SIZE/4) * i)];
				block[3 + ((KEY_SIZE/4) * i)] = block[3 + ((KEY_SIZE/4) * (i - 1))];
				block[3 + ((KEY_SIZE/4) * (i - 1))] = temp;
			}



            /*********************************************** { 구현 4 종료 } ********************************************/            
            break;

        case DEC:
            /*********************************************** { 구현 5 시작 } ********************************************/
			for(i = (KEY_SIZE/4) - 1; i > 0; i--) //row4
			{
				temp = block[1 + ((KEY_SIZE/4) * i)];
				block[1 + ((KEY_SIZE/4) * i)] = block[1 + ((KEY_SIZE/4) * (i - 1))];
				block[1 + ((KEY_SIZE/4) * (i - 1))] = temp;
			}

			for(i = 0; i < ((KEY_SIZE/4)/2); i++) // row 3
			{
				temp = block[2 + (i * (KEY_SIZE/4))];
				block[2 + (i * (KEY_SIZE/4))] = block[2 + ((KEY_SIZE/4) * (i + 2))];
				block[2 + ((KEY_SIZE/4) * (i + 2))] = temp;
			}
			for(i = 0; i < (KEY_SIZE/4) - 1; i++)// row 2
			{
				temp = block[3 + ((KEY_SIZE/4) * i)]; 
				block[3 + ((KEY_SIZE/4) * i)] = block[3 + ((KEY_SIZE/4) * (i + 1))];
				block[3 + ((KEY_SIZE/4) * (i+1))] = temp;
			}




            /*********************************************** { 구현 5 종료 } ********************************************/
            break;

        default:
            fprintf(stderr, "Invalid mode!\n");
            exit(1);
    }
    
    return block;
}

/*  <MixColumns 함수>
 *   
 *  block   MixColumns을 수행할 16바이트 블록. 수행 결과는 해당 배열에 바로 반영
 *  mode    MixColumns의 수행 모드
 */
BYTE* mixColumns(BYTE *block, int mode){    //***********
	int i,j,k;
	int sum = 0;
	int key_size = KEY_SIZE/4;
	BYTE block_copy[BLOCK_SIZE];
	BYTE temp;
	for(i = 0; i < BLOCK_SIZE; i++)
		block_copy[i] = block[i];
	for (i = 0; i < key_size-1; i++) {
		for (j = i + 1; j < key_size; j++) {
			temp = block_copy[4 * i + j];
			block_copy[4 * i + j] = block_copy[4 * j + i];
			block_copy[4 * j + i] = temp;
		}
	}
    /* 필요하다 생각하면 추가 선언 */  
    switch(mode){

        case ENC:
            /*********************************************** { 구현 6 시작 } ******************************************** MCOL*/
			for(i = 0; i < key_size ; i++)
			{
				for(j = 0; j < key_size ; j++)
				{
					sum = mul[MCOL[key_size * i] - 1][block_copy[j]];
					for(k = 1; k < key_size; k++)
						sum ^= mul[MCOL[key_size * i + k] - 1][block_copy[key_size * k + j]];
					*(block + (key_size * j) + i) = sum;
				}
			}
			
            /*********************************************** { 구현 6 종료 } ********************************************/            
            break;

        case DEC:
            /*********************************************** { 구현 7 시작 } ********************************************/
			for(i = 0; i < key_size ; i++)
			{
				for(j = 0; j < key_size ; j++)
				{
					sum = mul[RMCOL[key_size * i] - 1][block_copy[j]];
					for(k = 1; k < key_size; k++)
						sum ^= mul[RMCOL[key_size * i + k] - 1][block_copy[key_size * k + j]];
					*(block + (key_size * j) + i) = sum;
				}
			}

            /*********************************************** { 구현 7 종료 } ********************************************/
            break;

        default:
            fprintf(stderr, "Invalid mode!\n");
            exit(1);
    }
    
    return block;
}


/*  <AddRoundKey 함수>
 *   
 *  block   AddRoundKey를 수행할 16바이트 블록. 수행 결과는 해당 배열에 반영
 *  rKey    AddRoundKey를 수행할 16바이트 라운드키
 */
BYTE* addRoundKey(BYTE *block, BYTE *rKey){

    /*********************************************** { 구현 8 시작 } ********************************************/
	int i;
	
	for(i=0;i<BLOCK_SIZE;i++)
		block[i] = block[i] ^ rKey[i];



    /*********************************************** { 구현 8 종료 } ********************************************/

    return block;
}


/*  <128비트 AES 암호화 함수>
 *   
 *  plain   바이트 배열로 구성된 평문 (16바이트 고정)
 *  key     128비트 암호키 (16바이트)
 *
 *  @ret    암호화된 암호문
 */
BYTE* encrypt(BYTE *plain, BYTE *key){
    BYTE roundKey[ROUNDKEY_SIZE];
	//expandKey(BYTE *key, BYTE *roundKey) , subBytes(BYTE *block, int mode) , shiftRows(BYTE *block, int mode) , mixColumns(BYTE *block, int mode)
	// addRoundKey(BYTE *block, BYTE *rKey)
    /*********************************************** { 구현 9 시작 } ********************************************/
	int i,j;
	BYTE *cipher = (BYTE *)malloc(sizeof(BYTE)* BLOCK_SIZE);
	expandKey(key, roundKey);
	for(i=0;i<BLOCK_SIZE;i++)
		cipher[i]=plain[i];
//	printf("Round %d (input): ",0);
//	for(j = 0; j < 16; j++)
//		printf("%x",cipher[j]);
//	printf("\n\n");
	addRoundKey(cipher, roundKey);
	for(i=0;i<9;i++)
	{
//		printf("Round %d (start): ",(i+1));
//		for(j = 0; j < 16; j++)
//			printf("%x",cipher[j]);
//		printf("\n\n");
		subBytes(cipher, ENC);
//		printf("Round %d (subBytes): ",(i+1));
//		for(j = 0; j < 16; j++)
//			printf("%x",cipher[j]);
//		printf("\n\n");
		shiftRows(cipher, ENC);
//		printf("Round %d (shiftRows): ",(i+1));
//		for(j = 0; j < 16; j++)
//			printf("%x",cipher[j]);
//		printf("\n\n");
		mixColumns(cipher, ENC);
//		printf("Round %d (mixColumns): ",(i+1));
//		for(j = 0; j < 16; j++)
//			printf("%x",cipher[j]);
//		printf("\n\n");
		addRoundKey(cipher, &roundKey[(i+1)*BLOCK_SIZE]);
//		printf("Round %d (addRoundKey): ",(i+1));
//		for(j = 0; j < 16; j++)
//			printf("%x",cipher[j]);
//		printf("\n\n");
	}
	subBytes(cipher, ENC);
	shiftRows(cipher, ENC);
	addRoundKey(cipher, &roundKey[ROUNDKEY_SIZE-BLOCK_SIZE]);
//	for(i=0;i<16;i++)
//		printf("%d : %20x\n",(i+1),cipher[i]);
	return cipher;
    /*********************************************** { 구현 9 종료 } ********************************************/
}


/*  <128비트 AES 복호화 함수>
 *   
 *  cipher  바이트 배열로 구성된 평문 (16바이트 고정)
 *  key     128비트 암호키 (16바이트)
 *
 *  @ret    복호화된 평문
 */
BYTE* decrypt(BYTE *cipher, BYTE *key){
    BYTE roundKey[ROUNDKEY_SIZE];
    
    /*********************************************** { 구현 10 시작 } ********************************************/
	int i;
	BYTE *plain = (BYTE *)malloc(sizeof(BYTE)*BLOCK_SIZE);
	expandKey(key, roundKey);
	for(i=0;i<BLOCK_SIZE;i++)
		plain[i]=cipher[i];
	addRoundKey(plain, &roundKey[ROUNDKEY_SIZE-BLOCK_SIZE]);
	for (i=0;i<9;i++) {
		shiftRows(plain, DEC);
		subBytes(plain, DEC);
		addRoundKey(plain, &roundKey[ROUNDKEY_SIZE-BLOCK_SIZE*(i+2)]);
		mixColumns(plain, DEC);
	}
	shiftRows(plain, DEC);
	subBytes(plain, DEC);
	addRoundKey(plain, &roundKey[0]);
	return plain;
    /*********************************************** { 구현 10 종료 } ********************************************/
}


/*  <128비트 AES 암복호화 함수>
 *  
 *  mode가 ENC일 경우 평문을 암호화하고, DEC일 경우 암호문을 복호화하는 함수
 *
 *  [ENC 모드]
 *  plain   평문 바이트 배열
 *  cipher  결과(암호문)이 담길 바이트 배열. 호출하는 사용자가 사전에 메모리를 할당하여 파라미터로 넘김
 *  key     128비트 암호키 (16바이트)
 *
 *  [DEC 모드]
 *  plain   결과(평문)가 담길 바이트 배열. 호출하는 사용자가 사전에 메모리를 할당하여 파라미터로 넘김
 *  cipher  암호문 바이트 배열
 *  key     128비트 암호키 (16바이트)
 */
void AES128(BYTE *plain, BYTE *cipher, BYTE *key, int mode){
    BYTE *tmp;  

    if(mode == ENC){
        tmp = encrypt(plain, key);

        /*********************************************** { 구현 11 시작 } ********************************************/
		int i;
		for(i=0;i<16;i++)
			cipher[i]=tmp[i];


        /*********************************************** { 구현 11 종료 } ********************************************/ 

    }else if(mode == DEC){
        tmp = decrypt(cipher, key);

        /*********************************************** { 구현 12 시작 } ********************************************/
		int i;
		for(i=0;i<16;i++)
			plain[i]=tmp[i];


        /*********************************************** { 구현 12 종료 } ********************************************/ 

    }else{
        fprintf(stderr, "Invalid mode!\n");
        exit(1);
    }
}