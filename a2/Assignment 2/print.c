#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "assign2.h"

/**********************************************************************/
/*** Functions for printing BDD graph *********************************/
/*******************************************************ffff***************/
void SubGraph(t_blif_signal **inputs, FILE *fp, int u, bool *printed)
{

	int nameIdx = T[u].var;
	if (nameIdx < TotalNodes)
	{
		//set the name of this node
		printf("\t%d [label=\"%s\"];\n", u, inputs[nameIdx]->data.name);
		fprintf(fp, "\t%d [label=\"%s\"];\n", u, inputs[nameIdx]->data.name);
	}

	// print high subgraph
	if (false == printed[T[u].high])
	{
		SubGraph(inputs, fp, T[u].high, printed);
		// print the 1-path of this node
		printf("\t%d -> %d [1 Path];\n", u, T[u].high);
		fprintf(fp, "\t%d -> %d [label=\" 1\"];\n", u, T[u].high);
	}
	else
	{
		if ((T[u].high == 0) || (T[u].high == 1))
		{
			printf("\t%d -> %d [1 Path];\n", u, T[u].high);
			fprintf(fp, "\t%d -> %d [label=\" 1\"];\n", u, T[u].high);
		}

	}

	// print low subgraph

	if (false == printed[T[u].low])
	{
		SubGraph(inputs, fp, T[u].low, printed);
		printf("\t%d -> %d [0 Path];\n", u, T[u].low);
		fprintf(fp, "\t%d -> %d [label=\" 0\"];\n", u, T[u].low);
	}
	else
	{
		if ((T[u].low == 0) || (T[u].low == 1))
		{
			printf("\t%d -> %d [0 Path];\n", u, T[u].low);
			fprintf(fp, "\t%d -> %d [label=\" 0\"];\n", u, T[u].low);
		}
	}

	//set node as printed
	printed[u] = true;
}

void printDOTfromNode(t_blif_signal **inputs, int rootNode, char *bddName, int version)
{
	printf("there are %d inputs and %d nodes in total\n", InputCount, TRows);
	FILE *fp;
	char fileName[50]; // should be more than enough
	if (version == 0) {
		sprintf(fileName, "%s.dot", bddName);
	}
	else if (version == -1) {
		sprintf(fileName, "%s_nosift.dot", bddName);
	}
	else {
		sprintf(fileName, "%s_%d.dot", bddName, version);
	}
	fp = fopen(fileName, "w+");

	//temp bool to keep track of whether a node has been printed or not
	bool *printed = (bool*)malloc(TRows * sizeof(bool));
	memset(printed, false, TRows * sizeof(bool));

	printf("Graph %s {\n", bddName);
	fprintf(fp, "Graph %s {\n", bddName);


	SubGraph(inputs, fp, rootNode, printed);

	if (rootNode != 1)
	{
		printf("\t0 [Terminal Box];\n");
		fprintf(fp, "\t0 [Terminal Box];\n");
	}
	if (rootNode != 0)
	{
		printf("\t1 [Terminal Box];\n");
		fprintf(fp, "\t1 [Terminal];\n");
	}
	// free temp bool
	free(printed);

	printf("}\n");
	fprintf(fp, "}\n");
	fclose(fp);
}


void printDOT(t_blif_cubical_function *f, t_blif_signal **inputs, int BDDnum, bool isAllX)
{
	printf("there are %d inputs and %d nodes in total\n", InputCount, TRows);

	FILE *fp;
	char fileName[50]; // should be more than enough
	sprintf(fileName, "BDD%d.dot", BDDnum);
	fp = fopen(fileName, "w+");

	printf("Graph G {\n");
	fprintf(fp, "Graph G {\n");
	// 0 should be printed as long as it's not all DC
	if (!isAllX)
	{
		printf("\t0 [shape=box];\n");
		fprintf(fp, "\t0 [shape=box];\n");
	}
	if (f->cube_count > 0)
	{
		// 1 should always be there
		printf("\t1 [shape=box];\n");
		fprintf(fp, "\t1 [shape=box];\n");

		int i;
		//for(i=0; i < f->input_count; i++)
		for (i = 2; i<TRows; i++)
		{
			int var = T[i].var;
			int idx = f->inputs[var]->data.index;
			printf("input name = %s\n", inputs[idx]->data.name);
			printf("\t%d [label=\"%s\"];\n", i, inputs[idx]->data.name);
			fprintf(fp, "\t%d [label=\"%s\"];\n", i, inputs[idx]->data.name);
		}
		for (i = 2; i<TRows; i++)
		{
			// 0-path
			printf("\t%d -> %d [Terminal 0];\n", i, T[i].low);
			fprintf(fp, "\t%d -> %d [Terminal 0];\n", i, T[i].low);
			// 1-path
			printf("\t%d -> %d [Terminal 1];\n",
				i, T[i].high);
			fprintf(fp, "\t%d -> %d [Terminal 1];\n",
				i, T[i].high);
		}
	}
	printf("}\n");
	fprintf(fp, "}\n");
	fclose(fp);
}

void printTTable(TRow *T, t_blif_cubical_function *f)
{
	int i;
	printf("=====================================\n");
	printf("|Node\t|Variable\t|0 Path \t| 1 Path\t|swapped|\n");
	printf("|0\t|\n");
	printf("|1\t|\n");
	for (i = 2; i < TRows; i++) {
		printf("|%d\t|%d\t|%d\t|%d\t|%d\n", i, T[i].var, T[i].low, T[i].high, T[i].swapped);
	}
	printf("=====================================\n");
}