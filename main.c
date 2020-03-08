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

/* Znaky Alfanumericke */
const unsigned short chars_14seg[] = {
        0x003F, /* 0 */
				0x0006, /* 1 */
				0x111B, /* 2 */
        0x010F, /* 3 */
				0x1126, /* 4 */
				0x2069, /* 5 */
        0x00FD, /* 6 */
        0x0007, /* 7 */
        0x00FF, /* 8 */
        0x00EF, /* 9 */
        0x00F7, /* A */
        0x128F, /* B */
        0x0039, /* C */
        0x120F, /* D */
        0x0079, /* E */
        0x0071, /* F */
        0x0000  /* Space */
};
/* Definice typu Boolean */
typedef enum {
    false, true
} bool;

/* Definice typu rezim */
enum display {
    SMALL_SEG, BIG_SEG
};

/* Nazev ovladaneho displeje */
const char *nazev_displeje[] = {"Numericky", "Alfanumericky"};

/* pole masek pro separaci jednotlivych bitu */
const unsigned short bit_masks[WORD_SIZE] =
        {1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7, 1 << 8, 1 << 9, 1 << 10, 1 << 11, 1 << 12,
         1 << 13, 1 << 14, 1 << 15};

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
		output_buffer &= ~(1 << CLK);
		outportb(OUTPUT_PORT, output_buffer);
		output_buffer |= 1 << CLK;
    outportb(OUTPUT_PORT, output_buffer);
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
								output_buffer |= 1 << DATA;
								//outportb(OUTPUT_PORT, output_buffer);
						} else {
								output_buffer &= ~(1 << DATA);
								//outportb(OUTPUT_PORT, output_buffer);
						}

						tick(); /* Odesli bit ven */
				}
		}
}

void
shiftSWbig(const unsigned short *sequence_array)
{
		int opakovani;
		/* projdeme vsechny pozice displeje */
		for (opakovani = 0; opakovani < 2; opakovani++) {
				int pozice;
				int i;
				sendStartBit();

				for (pozice = opakovani; pozice < BIG_DISP_POZICE; pozice += 2) {
						/* projdi jednotlive bity a odesli posli je do realneho shift registru */
						int bit;
						for (bit = 0; bit < BIG_DISP_SIZE; bit++) {
								unsigned char test;
								/* Test jestli je bit v log. 1 nebo 0 */
								if (sequence_array[pozice] & bit_masks[bit]) {
										output_buffer |= 1 << DATA;
										test = '1';
										//outportb(OUTPUT_PORT, output_buffer);
								} else {
										output_buffer &= ~(1 << DATA);
										test = '0';
										//outportb(OUTPUT_PORT, output_buffer);
								}
								printf("%c", test);
								tick(); /* Odesli bit ven */
                
						}
						delay(1);
				}
				/* Urceni pozice */
				if (opakovani == 0) {
						output_buffer |= 1 << DATA;
						outportb(OUTPUT_PORT, output_buffer);
						printf("1");
						tick();
						output_buffer &= ~(1 << DATA);
						outportb(OUTPUT_PORT, output_buffer);
						printf("0");
						tick();
				} else {
						output_buffer &= ~(1 << DATA);
						outportb(OUTPUT_PORT, output_buffer);
						printf("0");
						tick();
						output_buffer |= 1 << DATA;
						outportb(OUTPUT_PORT, output_buffer);
						printf("1");
						tick();
				}
				/* bity bez vyznamu */
				for (i = 0; i < 3; i++) {
					printf("X");
					tick();
				}
				printf("\n");
		}

}

void
sendStartBit(void)
{
		/* Start Bit */
		output_buffer |= 1 << DATA;
		outportb(OUTPUT_PORT, output_buffer);
		printf("S");
		tick(); /* Odesli start bit */
}

int
main(void)
{
		/* init */
		bool program_run;
		void (*shiftSW)(const unsigned short *);    /*ukazatel na aktualni funkci posuvneho registru */
		enum display rezim = SMALL_SEG;
		unsigned char cursor = 0;
		unsigned short zobrazovane_znaky[5];
		zobrazovane_znaky[0] = chars_7seg[16];
		zobrazovane_znaky[1] = chars_7seg[16];
		zobrazovane_znaky[2] = chars_7seg[16];
		zobrazovane_znaky[3] = chars_7seg[16];
		zobrazovane_znaky[4] = chars_7seg[16];

		/* Program zacina zde */
		/* vycisteni dat na portu */
		output_buffer = NEAKTIVITA;
		outportb(OUTPUT_PORT, output_buffer);

		/* default stav shiftSW */
		shiftSW = shiftSWsmall;

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
				printf("Zapises na %d. pozici \n", cursor + 1);

				printf("0123456789012345678901234567890123456 \n");

				if (kbhit())
						hitted_key = getch();

				switch (hitted_key) {
						case ESC:
								program_run = false;
								break;
						case '0':
								zobrazovane_znaky[cursor] = rezim == SMALL_SEG ? chars_7seg[0] : chars_14seg[0];
								cursor++;
								hitted_key = NULL;
								break;
						case '1':
								zobrazovane_znaky[cursor] = rezim == SMALL_SEG ? chars_7seg[1] : chars_14seg[1];
								cursor++;
								hitted_key = NULL;
								break;
						case '2':
								zobrazovane_znaky[cursor] = rezim == SMALL_SEG ? chars_7seg[2] : chars_14seg[2];
								cursor++;
								hitted_key = NULL;
								break;
						case '3':
								zobrazovane_znaky[cursor] = rezim == SMALL_SEG ? chars_7seg[3] : chars_14seg[3];
								cursor++;
								hitted_key = NULL;
								break;
						case '4':
								zobrazovane_znaky[cursor] = rezim == SMALL_SEG ? chars_7seg[4] : chars_14seg[4];
								cursor++;
								hitted_key = NULL;
								break;
						case '5':
								zobrazovane_znaky[cursor] = rezim == SMALL_SEG ? chars_7seg[5] : chars_14seg[5];
								cursor++;
								hitted_key = NULL;
								break;
						case '6':
								zobrazovane_znaky[cursor] = rezim == SMALL_SEG ? chars_7seg[6] : chars_14seg[6];
								cursor++;
								hitted_key = NULL;
								break;
						case '7':
								zobrazovane_znaky[cursor] = rezim == SMALL_SEG ? chars_7seg[7] : chars_14seg[7];
								cursor++;
								hitted_key = NULL;
								break;
						case '8':
								zobrazovane_znaky[cursor] = rezim == SMALL_SEG ? chars_7seg[8] : chars_14seg[8];
								cursor++;
								hitted_key = NULL;
								break;
						case '9':
								zobrazovane_znaky[cursor] = rezim == SMALL_SEG ? chars_7seg[9] : chars_14seg[9];
								cursor++;
								hitted_key = NULL;
								break;
						case 'a':
								zobrazovane_znaky[cursor] = rezim == SMALL_SEG ? chars_7seg[10] : chars_14seg[10];
								cursor++;
								hitted_key = NULL;
								break;
						case 'b':
								zobrazovane_znaky[cursor] = rezim == SMALL_SEG ? chars_7seg[11] : chars_14seg[11];
								cursor++;
								hitted_key = NULL;
								break;
						case 'c':
								zobrazovane_znaky[cursor] = rezim == SMALL_SEG ? chars_7seg[12] : chars_14seg[12];
								cursor++;
								hitted_key = NULL;
								break;
						case 'd':
								zobrazovane_znaky[cursor] = rezim == SMALL_SEG ? chars_7seg[13] : chars_14seg[13];
								cursor++;
								hitted_key = NULL;
								break;
						case 'e':
								zobrazovane_znaky[cursor] = rezim == SMALL_SEG ? chars_7seg[14] : chars_14seg[14];
								cursor++;
								hitted_key = NULL;
								break;
						case 'f':
								zobrazovane_znaky[cursor] = rezim == SMALL_SEG ? chars_7seg[15] : chars_14seg[15];
								cursor++;
								hitted_key = NULL;
								break;
						case SPACE:
								zobrazovane_znaky[cursor] = rezim == SMALL_SEG ? chars_7seg[16] : chars_14seg[16];
								cursor++;
								hitted_key = NULL;
								break;
						case BACKSPACE:
								if (cursor == 0)
										cursor = rezim == SMALL_SEG ? 5 : 4;
								zobrazovane_znaky[--cursor] = chars_7seg[16];
								hitted_key = NULL;
								break;
						case TAB:
								if (rezim == SMALL_SEG) {
										cursor = 0;
										rezim = BIG_SEG;
										shiftSW = shiftSWbig;
								} else {
										cursor = 0;
										rezim = SMALL_SEG;
										shiftSW = shiftSWsmall;
								}
								hitted_key = NULL;
								break;

				}
				/* osetreni kurzoru  */
				if (rezim == SMALL_SEG) {
						if (cursor > SMALL_DISP_POZICE - 1)
								cursor = 0;
				} else {
						if (cursor > BIG_DISP_POZICE - 1)
								cursor = 0;
				}


				shiftSW(zobrazovane_znaky);

				//delay(70);
		} while (program_run);

		return 0;
}
