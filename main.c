#include <stdio.h>
#include <stdlib.h>
//#include <dos.h>

#define BYTE_SIZE 8

/* Definice typu Boolean */
typedef enum {false, true} bool;
/* Definice typu rezim */
enum display {SMALL_SEG, BIG_SEG};

/* pole masek pro separaci jednotlivych bitu */
const unsigned short bit_masks[2][BYTE_SIZE] = {
        {1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7},
        {1 << 8, 1 << 9, 1 << 10, 1 << 11, 1 << 12, 1 << 13, 1 << 14, 1 << 15}
};

/* Deklarace funkci */
void shiftSW(unsigned short *sequence_array, unsigned char pocet_pozic, enum display rezim);

/*
 * Program posuvny registr
 */
void
shiftSW(unsigned short *sequence_array, unsigned char pocet_pozic, enum display rezim)
{
    /* projdeme vsechny pozice displeje */
    int pozice;
    for (pozice = 0; pozice < pocet_pozic; ++pozice) {
        /* projdi jednotlive bity a odesli posli je do realneho shift registru */
        int ramec;
        for (ramec = 0; ramec <= rezim; ++ramec) {
            int bit;
            for (bit = 0; bit < BYTE_SIZE; ++bit) {
                /* Test jestli je bit v log. 1 nebo 0 */
                if (sequence_array[pozice] & bit_masks[ramec][bit])
                    printf("1"); /*TODO outportb()*/
                else
                    printf("0");

                /*TODO Tick*/
            }
        }
        printf("\n");
    }
}


int
main(void)
{
    /* Program zacina zde */


    /* vycisteni obrazovky*/
//  clrscr()

    /* dokud je true, program se cyklicky vykonava */
    bool program_run = true;

    do {
        /* Nekonecna smycka programu */
        unsigned short pole[] = {0xFF2F, 0x3F, 0xFF, 0x00, 0x24};
        shiftSW(pole, 5, 1);
        program_run = false;

    } while (program_run);

    return 0;
}
