#include <stdio.h>       
#include <stdlib.h>
#include <dos.h>
#include <conio.h>

/* velikosti ramcu */
#define WORD_SIZE 16
#define SMALL_DISP_SIZE 7
#define SMALL_DISP_POZICE 5
#define BIG_DISP_SIZE 15
#define BIG_DISP_POZICE 4

/* Konstanty vystupniho portu */
#define OUTPUT_PORT 0x300
#define CLK 1
#define DATA 0
#define NEAKTIVITA 0xFF

/* Pojmenovani nic nerikajicich cisel */
#define ESC 0x1B
#define SPACE 0x20
#define BACKSPACE 0x08
#define TAB 0x09

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
				0x71, /* F */
				0x00  /* Space */
};

/* Definice typu Boolean */
typedef enum {false, true} bool;
/* Definice typu rezim */
enum display {SMALL_SEG, BIG_SEG};

/* Nazev ovladaneho displeje */
const char *nazev_displeje[] = {"Numericky", "Alfanumericky"};

/* pole masek pro separaci jednotlivych bitu */
const unsigned short bit_masks[WORD_SIZE] =
				{1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7, 1 << 8, 1 << 9, 1 << 10, 1 << 11, 1 << 12, 1 << 13, 1 << 14, 1 << 15};

/*Deklarace globalnich promennych*/
static unsigned char output_buffer;

/* Deklarace funkci */
void tick(void);
void shiftSWsmall(const unsigned short *sequence_array);
void shiftSWbig(const unsigned short *sequence_array);
void sendStartBit(void);

/*
 * Hodinovy signal
 */
void
tick(void)
{
		output_buffer &= ~(1<<CLK);
		outportb(OUTPUT_PORT, output_buffer);
		output_buffer |= 1<<CLK;
}

/*
 * Program posuvny registr
 */
void
shiftSWsmall(const unsigned short *sequence_array)
{
		int pozice;

		sendStartBit();

		/* projdeme vsechny pozice displeje */
		for (pozice = 0; pozice < SMALL_DISP_POZICE; pozice++) {
				/* projdi jednotlive bity a odesli posli je do realneho shift registru */
				int bit;
				for (bit = 0; bit < SMALL_DISP_SIZE; bit++) {

						/* Test jestli je bit v log. 1 nebo 0 */
						if (sequence_array[pozice] & bit_masks[bit]) {
								output_buffer |= 1<<DATA;
								outportb(OUTPUT_PORT, output_buffer);
						} else {
								output_buffer &= ~(1<<DATA);
								outportb(OUTPUT_PORT, output_buffer);
						}

						tick(); /* Odesli bit ven */
				}
		}
}

void
shiftSWbig(const unsigned short *sequence_array)
{
    //int opakovani;
    /* projdeme vsechny pozice displeje */
    for (int opakovani = 0; opakovani < 2; opakovani++) {
        //int pozice;
        sendStartBit();

        for (int pozice = opakovani; pozice < BIG_DISP_POZICE; pozice+=2) {
            /* projdi jednotlive bity a odesli posli je do realneho shift registru */
            int bit;
            for (bit = 0; bit < BIG_DISP_SIZE; bit++) {

                /* Test jestli je bit v log. 1 nebo 0 */
                if (sequence_array[pozice] & bit_masks[bit]) {
                    output_buffer |= 1<<DATA;
                    outportb(OUTPUT_PORT, output_buffer);
                } else {
                    output_buffer &= ~(1<<DATA);
                    outportb(OUTPUT_PORT, output_buffer);
                }

                tick(); /* Odesli bit ven */
            }
        }
        /* dopln zbivajici bity */
        /* Urceni pozice */

        /* bity bez vyznamu */


        delay(10); /* Pauza pro ocko */
    }

}

void
sendStartBit(void)
{
    /* Start Bit */
    output_buffer |= 1<<DATA;
    outportb(OUTPUT_PORT, output_buffer);

    tick(); /* Odesli start bit */
}

int
main(void)
{
		bool program_run;
		enum display rezim = SMALL_SEG;
		unsigned char cursor = 0;
		unsigned short zobrazovane_znaky[5];
        zobrazovane_znaky[0] = chars_7seg[10];
        zobrazovane_znaky[1] = chars_7seg[15];
        zobrazovane_znaky[2] = chars_7seg[15];
        zobrazovane_znaky[3] = chars_7seg[15];
        zobrazovane_znaky[4] = chars_7seg[15];

		/* Program zacina zde */
		/* vycisteni dat na portu */
		output_buffer = NEAKTIVITA;
		outportb(OUTPUT_PORT, output_buffer);

		/* vycisteni obrazovky*/
		clrscr();

		/* dokud je true, program se cyklicky vykonava */
		program_run = true;


		do {
				int hitted_key;

				/* Nekonecna smycka programu */

				/* Informace na konzoli */
				clrscr();
				printf("Ovladas %s displej \n", nazev_displeje[rezim]);
				printf("Zapises na %d. pozici \n", cursor+1);

				if (kbhit()) {
					hitted_key = getch();
				}

				switch (hitted_key) {
				case ESC:
					program_run = false;
					break;
				case '0':
                    zobrazovane_znaky[cursor] = chars_7seg[0];
					cursor++;
					hitted_key = NULL;
					break;
				case '1':
                    zobrazovane_znaky[cursor] = chars_7seg[1];
					cursor++;
					hitted_key = NULL;
					break;
				case '2':
                    zobrazovane_znaky[cursor] = chars_7seg[2];
					cursor++;
					hitted_key = NULL;
					break;
				case '3':
                    zobrazovane_znaky[cursor] = chars_7seg[3];
					cursor++;
					hitted_key = NULL;
					break;
				case '4':
                    zobrazovane_znaky[cursor] = chars_7seg[4];
					cursor++;
					hitted_key = NULL;
					break;
				case '5':
                    zobrazovane_znaky[cursor] = chars_7seg[5];
					cursor++;
					hitted_key = NULL;
					break;
				case '6':
                    zobrazovane_znaky[cursor] = chars_7seg[6];
					cursor++;
					hitted_key = NULL;
					break;
				case '7':
                    zobrazovane_znaky[cursor] = chars_7seg[7];
					cursor++;
					hitted_key = NULL;
					break;
				case '8':
                    zobrazovane_znaky[cursor] = chars_7seg[8];
					cursor++;
					hitted_key = NULL;
					break;
				case '9':
                    zobrazovane_znaky[cursor] = chars_7seg[9];
					cursor++;
					hitted_key = NULL;
					break;
				case 'a':
                    zobrazovane_znaky[cursor] = chars_7seg[10];
					cursor++;
					hitted_key = NULL;
					break;
				case 'b':
                    zobrazovane_znaky[cursor] = chars_7seg[11];
					cursor++;
					hitted_key = NULL;
					break;
				case 'c':
                    zobrazovane_znaky[cursor] = chars_7seg[12];
					cursor++;
					hitted_key = NULL;
					break;
				case 'd':
                    zobrazovane_znaky[cursor] = chars_7seg[13];
					cursor++;
					hitted_key = NULL;
					break;
				case 'e':
                    zobrazovane_znaky[cursor] = chars_7seg[14];
					cursor++;
					hitted_key = NULL;
					break;
				case 'f':
                    zobrazovane_znaky[cursor] = chars_7seg[15];
					cursor++;
					hitted_key = NULL;
					break;
				case SPACE:
                    zobrazovane_znaky[cursor] = chars_7seg[16];
					cursor++;
					hitted_key = NULL;
					break;
				case BACKSPACE:
					if (cursor == 0)
						cursor = 5;
                        zobrazovane_znaky[--cursor] = chars_7seg[16];
					hitted_key = NULL;
					break;

				}
				/* osetreni kurzoru  */
				if (cursor > SMALL_DISP_POZICE - 1)
					cursor = 0;

				shiftSWsmall(zobrazovane_znaky);

				/*delay(100);*/
		} while (program_run);

		return 0;
}
