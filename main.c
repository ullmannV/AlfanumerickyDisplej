#include <stdio.h>
#include <stdlib.h>
#include <dos.h>


/* velikosti ramcu */
#define WORD_SIZE 16
#define SMALL_DISP_SIZE 8
#define BIG_DISP_SIZE 14

/* Konstanty vystupniho portu */
#define OUTPUT_PORT 0x300
#define CLK 1
#define DATA 0
#define NEAKTIVITA 0xFF

/* Znaky 7seg */
const unsigned char chars_7seg[] = {
        0x3F, /* 0 */
        0x06, /* 1 */
        0x5B, /* 2 */
        0x4F, /* 3 */
        0x66, /* 4 */
        0x6D, /* 5 */
        0x7D, /* 6 */
        0x07, /* 7 */
        0x7F, /* 8 */
        0x6F, /* 9 */
        0x77, /* A */
        0x7C, /* b */
        0x39, /* C */
        0x5E, /* d */
        0x79, /* E */
        0x71  /* F */
};

/* Definice typu Boolean */
typedef enum {false, true} bool;
/* Definice typu rezim */
enum display {SMALL_SEG, BIG_SEG};

/* pole masek pro separaci jednotlivych bitu */
const unsigned short bit_masks[WORD_SIZE] =
        {1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7, 1 << 8, 1 << 9, 1 << 10, 1 << 11, 1 << 12, 1 << 13, 1 << 14, 1 << 15};

/*Deklarace globalnich promennych*/
static unsigned char output_buffer;

/* Deklarace funkci */
static inline void tick(void);
void shiftSW(unsigned short *sequence_array, unsigned char pocet_pozic, enum display rezim);


/*
 * Hodinovy signal
 */
static inline void
tick(void)
{
    output_buffer |= 1<<CLK;
    outportb(OUTPUT_PORT, output_buffer);
    output_buffer &= ~(1<<CLK);
}

/*
 * Program posuvny registr
 */
void
shiftSW(unsigned short *sequence_array, unsigned char pocet_pozic, enum display rezim)
{
    /* Start Bit */
    output_buffer |= 1<<DATA;
    outportb(OUTPUT_PORT, output_buffer);

    tick();

    /* projdeme vsechny pozice displeje */
    int pozice;
    for (pozice = 0; pozice < pocet_pozic; ++pozice) {
        /* projdi jednotlive bity a odesli posli je do realneho shift registru */
        int bit;
        for (bit = 0; bit < SMALL_DISP_SIZE; ++bit) {
            /* Test jestli je bit v log. 1 nebo 0 */
            if (sequence_array[pozice] & bit_masks[bit]) {
                printf("1");
                output_buffer |= 1<<DATA;
                outportb(OUTPUT_PORT, output_buffer);
            } else {
                printf("0");
                output_buffer &= ~(1<<DATA);
                outportb(OUTPUT_PORT, output_buffer);
            }

            tick();
        }

        printf("\n");
    }
}


int
main(void)
{
    /* Program zacina zde */
    output_buffer = NEAKTIVITA;

    /* vycisteni obrazovky*/
//  clrscr()

    /* dokud je true, program se cyklicky vykonava */
    bool program_run = true;

    do {
        /* Nekonecna smycka programu */
//      clrscr();
        unsigned short pole[] = {chars_7seg[0], chars_7seg[1], chars_7seg[10], chars_7seg[8], chars_7seg[15]};
        shiftSW(pole, 5, 0);
        program_run = false;

    } while (program_run);

    return 0;
}
