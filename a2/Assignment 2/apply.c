#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include "common_types.h"
#include "blif_common.h"
#include "cubical_function_representation.h"
#include "assign2.h"
#include "apply.h"
#include <windows.h>


int translateOp(char* op)
{
	if (!strcmp(op, "AND") || !strcmp(op, "and"))
		return AND;
	if (!strcmp(op, "OR") || !strcmp(op, "or"))
		return OR;
	if (!strcmp(op, "XOR") || !strcmp(op, "xor"))
		return XOR;
	if (!strcmp(op, "NAND") || !strcmp(op, "nand"))
		return NAND;
	if (!strcmp(op, "NOR") || !strcmp(op, "nor"))
		return NOR;
	if (!strcmp(op, "XNOR") || !strcmp(op, "xnor"))
		return XNOR;
	return -1;
}
//Perform Apply operation
int perfOp(int op, int u1, int u2)
{
    if(AND == op)
        return u1 && u2;
    else if(OR == op)
        return u1 || u2;
    else if(XOR == op)
        return u1 != u2;
    else if(NAND == op)
        return !(u1 && u2);
    else if(NOR == op)
        return !(u1 || u2);
    else if(XNOR == op)
        return !(u1 != u2);
    return 0;
}

int appOp(TRow *T,int op, int u1, int u2)
{
	int u;

	if (G[u1][u2] != -1)
		return G[u1][u2];
	else if (((u1 == 0) || (u1 == 1)) && ((u2 == 0) || (u2 == 1)))
		u = perfOp(op, u1, u2);
	else if (T[u1].var == T[u2].var)
		u = MK(T[u1].var, appOp(T,op, T[u1].low, T[u2].low), appOp(T,op, T[u1].high, T[u2].high));
	else if (T[u1].var < T[u2].var)
		u = MK(T[u1].var, appOp(T,op, T[u1].low, u2), appOp(T,op, T[u1].high, u2));
	else
		u = MK(T[u2].var, appOp(T,op, u1, T[u2].low), appOp(T,op, u1, T[u2].high));

	G[u1][u2] = u;
	return u;
}


void appMain(TRow *T, t_blif_logic_circuit *circuit, int bddNum)
{
	char applyCmd[100];
	int op = -1;
	int node1 = -1;
	int node2 = -1;

	LARGE_INTEGER frequency;        // ticks per second
	LARGE_INTEGER t1, t2;           // ticks
	double elapsedTime;
	while (1)
	{
		printf("Apply Operation Starts...\n");
		printf("Enter boolean command and two functions for operation:\n");
		printf("Apply>");
		fgets(applyCmd, 100, stdin);
	//	printf("received: %s\n", applyCmd);
		printf("\n");
		printf("Resulting BDD:\n");
		if (!strcmp("exit\n", applyCmd))
			break;

		// parse command
		char *node1Str = strtok(applyCmd, " \n");
		if (node1Str != NULL)
			node1 = findOutput(circuit, node1Str);
		char *opStr = strtok(NULL, " \n");
		if (opStr != NULL)
			op = translateOp(opStr);
		char *node2Str = strtok(NULL, " \n");
		if (node2Str != NULL)
			node2 = findOutput(circuit, node2Str);
		////////////////do apply////////////////////
		clearG();
		// get ticks per second
		QueryPerformanceFrequency(&frequency);

		// start timer
		QueryPerformanceCounter(&t1);

		int applyRoot = appOp(T, op, node1, node2);
		QueryPerformanceCounter(&t2);
		// compute and print the elapsed time in millisec
		elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
		printf("Time taken for apply operation = %f ms\n", elapsedTime);
		char bddName[50];
		sprintf(bddName, "%s_%s_%s", node1Str, opStr, node2Str);
		printDOTfromNode(circuit->primary_inputs, applyRoot, bddName, 0);
	}
}
