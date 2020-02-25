#include <stdio.h>
#include <stdlib.h>
//#include <dos.h>

/* Definice typu Boolean */
typedef enum {false, true} bool;

/* Deklarace funkci */
void shiftSW(unsigned char *sequence_array);

/*
 * Program posuvny registr
 */
void
shiftSW(unsigned char *sequence_array)
{

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
        program_run = false;
    } while (program_run);

    return 0;
}
