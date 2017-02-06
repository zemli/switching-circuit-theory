#include <time.h>
#include "cubical_function_representation.h"
#include <string.h>
#include <stdio.h>


bool BuildBDD;
int SiftOperation;
int **G;


/**********************************************************************/
/*** DEFINE STATEMENTS ************************************************/
/**********************************************************************/
#define INIT_SIZE 1024
#define AND  0x100
#define OR   0x101
#define XOR  0x102
#define NAND 0x103
#define NOR  0x104
#define XNOR 0x105


typedef struct _TRow {
	int var;
	int low;
	int high;
	bool swapped; // a bool for whether the entry has been swapped already
} TRow;
