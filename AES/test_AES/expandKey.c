#include <stdio.h>
#include <stdlib.h>
#include "AES128.h"

#define KEY_SIZE 16
#define ROUNDKEY_SIZE 176
#define BLOCK_SIZE 16

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
	0x1b, 0x00, 0x00, 0x00,
	0x36, 0x00, 0x00, 0x00
};

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


int main()
{
	BYTE key[] = 
	{ 
		0x2b, 0x7e, 0x15, 0x16,
		0x28, 0xae, 0xd2, 0xa6, 
		0xab, 0xf7, 0x15, 0x88,
		0x09, 0xcf, 0x4f, 0x3c
	};
	BYTE roundKey[ROUNDKEY_SIZE];
	expandKey(key,roundKey);
	int i;
	for(i = 0; i < 176; i++)
		printf("%d . %20x\n",(i+1),roundKey[i]);
	return 0;
}