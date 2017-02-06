
/**********************************************************************/
/*** HEADER FILES *****************************************************/
/**********************************************************************/

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
#include "findPI.h"
#include "apply.h"
#include <windows.h>


/**********************************************************************/
/*** GLOBAL VARIABLES *************************************************/
/**********************************************************************/
int TotalNodes = 0;
int InputCount = 0;
TRow *T;
int InitialTRow = INIT_SIZE;
int TRows = 0;
int **G;
int InitialGRow = INIT_SIZE;
t_blif_cubical_function *curFunc;
int *outRoot;
int rNodeIdx;
LARGE_INTEGER frequency;        // ticks per second
LARGE_INTEGER t1, t2;           // ticks
double elapsedTime;
int row1 = 0;


/**********************************************************************/
/*** Functions for printing BDD graph *********************************/
/*******************************************************ffff***************/
void SubGraph(t_blif_signal **inputs, FILE *fp, int u, bool *printed)
{
  
    int nameIdx = T[u].var;
    if(nameIdx < TotalNodes)
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
		printf("\t%d -> %d [High Path];\n", u, T[u].high);
		fprintf(fp, "\t%d -> %d [label=\" 1\"];\n", u, T[u].high);
	}
		else
		{
			if ((T[u].high == 0) || (T[u].high == 1))
			{
				printf("\t%d -> %d [High Path];\n", u, T[u].high);
				fprintf(fp, "\t%d -> %d [label=\" 1\"];\n",u, T[u].high);
			}

		}
	
// print low subgraph

	if (false == printed[T[u].low])
	{
		SubGraph(inputs, fp, T[u].low, printed);
		printf("\t%d -> %d [Low Path];\n", u, T[u].low);
		fprintf(fp, "\t%d -> %d [label=\" 0\"];\n", u, T[u].low);
	}
	else
	{
		if ((T[u].low == 0) || (T[u].low == 1))
		{
			printf("\t%d -> %d [Low Path];\n", u, T[u].low);
			fprintf(fp, "\t%d -> %d [label=\" 0\"];\n", u, T[u].low);
		}
	}
	
	//set node as printed
    printed[u] = true;
}

void printDOTfromNode(t_blif_signal **inputs, int rootNode, char *bddName, int version)
{
//	TRows = numValidRows(T);
    printf("Graph %s has %d inputs and %d nodes in total\n", bddName, InputCount, TRows);
    FILE *fp;
    char fileName[50]; // should be more than enough
    if (version == 0) {
        sprintf(fileName, "%s.dot", bddName);
    } else if (version == -1) {
        sprintf(fileName, "%s_nosift.dot", bddName);
    } else {
        sprintf(fileName, "%s_%d.dot", bddName, version);
    }
    fp = fopen(fileName, "w+");
	
    //temp bool to keep track of whether a node has been printed or not
    bool *printed = (bool*) malloc (TRows * sizeof(bool));
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

    printf("}\n\n");
    fprintf(fp, "}\n\n");
    fclose(fp);
}

void printDOTfromNode2(t_blif_signal **inputs, int rootNode, char *bddName, int version)
{
	//	TRows = numValidRows(T);
	printf("Graph %s has %d inputs and %d nodes in total\n", bddName, InputCount, TRows-row1+2);
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

	printf("}\n\n");
	fprintf(fp, "}\n\n");
	fclose(fp);
}

int printTTable (TRow *T, t_blif_cubical_function *f)
{
    int i;
    printf("=====================================\n");
    printf("|Node\t|Var\t|0 Path\t| 1 Path\n");
    printf("|0\t|\n");
    printf("|1\t|\n");
    for (i = 2; i < TRows; i++) {
     printf("|%d\t|%d\t|%d\t|%d\t\n", i, T[i].var, T[i].low, T[i].high);
    	}
    printf("=====================================\n");
	return TRows;
}

void printTTable2(TRow *T, t_blif_cubical_function *f)
{
	int i;
	printf("=====================================\n");
	printf("|Node\t|Var\t|0 Path\t| 1 Path\n");
	printf("|0\t|\n");
	printf("|1\t|\n");
	for (i = row1; i < TRows; i++) {
		printf("|%d\t|%d\t|%d\t|%d\t\n", i, T[i].var, T[i].low, T[i].high);
	}
	printf("=====================================\n");
}

void printVarOrder(int *varOrder, int size) {
	int i;
//	printf("Variable Ordering ");
	for (i = 0; i < size; i++) {

		printf("%d\t", varOrder[i]);
	}
	printf("\n");
}

/**********************************************************************/
/*** Functions for G **************************************************/
/**********************************************************************/

//---------------------------------------------------------------------
// Initialize the table G 
//---------------------------------------------------------------------
void initG()
{
    G = (int **) malloc(INIT_SIZE * sizeof(int *));
    int i;
    for(i=0; i<INIT_SIZE; i++)
    {
        G[i] = (int *) malloc(INIT_SIZE * sizeof(int));
        // initialize all value to -1 (empty)
        memset(G[i], -1, INIT_SIZE * sizeof(int));
    }
}

//---------------------------------------------------------------------
// Deallocate the space assigned to table G
//---------------------------------------------------------------------
void freeG()
{
    int i;
    for(i=0; i<InitialGRow; i++)
    {
        free(G[i]);
    }
    free(G);
}

//---------------------------------------------------------------------
// Empty table G 
//---------------------------------------------------------------------
void clearG()
{
    int i;
    for(i=0; i<InitialGRow; i++)
    {
        memset(G[i], -1, INIT_SIZE * sizeof(int));
    }
}

//---------------------------------------------------------------------
// increase the size of table G
//---------------------------------------------------------------------
void UpdateG(int size)
{
    G = (int **) realloc(G, size * sizeof(int *));
    // initialize all value to -1 (empty)
    int i, j;
    for(i=0; i<size; i++) {
        if(i<InitialGRow)
            G[i] = (int *) realloc(G[i], size * sizeof(int));
        else
            G[i] = (int *) malloc(size * sizeof(int));

        for(j=0; j<size; j++) {
            if((i>=InitialGRow) || (j>=InitialGRow))
                G[i][j] = -1;
        }
    }
    InitialGRow = size;
}


/**********************************************************************/
/*** Functions for T **************************************************/
/**********************************************************************/

//---------------------------------------------------------------------
// Initialize the table T with the two terminal nodes, 0 and 1
// each row of table T is one node
//---------------------------------------------------------------------
void initT()
{
    // reserve space for terminal nodes only
    T = (TRow*) malloc(INIT_SIZE * sizeof(TRow));
    // initialize value for the terminal nodes
    T[0].var = TotalNodes; // terminal node 0
    T[0].low = -1;
    T[0].high = -1;
    T[1].var = TotalNodes; // terminal node 1
    T[1].low = -1;
    T[1].high = -1;
    TRows = 2;
}


//---------------------------------------------------------------------
// count the number of valid rows in T
//---------------------------------------------------------------------
int numValidRows(TRow *T)
{
    int i;
    int res = 2;
    for (i = 2; i < TRows; i++) {
        if (T[i].var >= 0 && T[i].var < InputCount && T[i].low >= 0 && T[i].low < TRows
            && T[i].high >= 0 && T[i].high < TRows) 
			
			res++;
    }
    return res;
}


int MK(int i, int l, int h)
{
	if (l == h)
		return l;
	else if (member(i, l, h))
		return lookup(i, l, h);
	else
	{
		int u = add(i, l, h);
		return u;
	}
}

bool member(int i, int l, int h)
{
    bool found = false;
    int idx;
    for(idx=0; (idx < TRows) && (!found); idx++)
    {
        if( (T[idx].var == i) && (T[idx].low == l) && (T[idx].high == h) )
        {
            found = true;
            break;
        }
    }
    return found;
}


int lookup(int i, int l, int h)
{
    bool found = false;
    int idx;
    for(idx=0; (idx < TRows) && (!found); idx++)
    {
        if( (T[idx].var == i) && (T[idx].low == l) && (T[idx].high == h) )
        {
            found = true;
            break;
        }
    }
    if(found)
        return idx;
    else
        return -1; //always call member() before this, so should never reach here
}

int add(int i, int l, int h)
{
	int idx = TRows;
	TRows++;

	if (TRows > InitialTRow)
	{
		InitialTRow += INIT_SIZE;
		T = (TRow*)realloc(T, InitialTRow);
		UpdateG(InitialTRow);
	}

	T[idx].var = i;
	T[idx].low = l;
	T[idx].high = h;
	T[idx].swapped = false;
	return idx;
}

//---------------------------------------------------------------------
// Build BDD
//---------------------------------------------------------------------

int build(t_blif_cube **setOfCubes, int cubeCount, int i, int literalVal)
{
    if(i == InputCount)
    {
        if(literalVal == LITERAL_0)
            return 0;
        else
            return 1;
    }
    else
    {
        t_blif_cube **v0Child = (cubeCount==0)?(NULL):
            ((t_blif_cube **) malloc(cubeCount * sizeof(t_blif_cube *))); 
        int v0ChildCount = 0;
        int v0Literal = Cofactor(setOfCubes, cubeCount, i, LITERAL_0,
                            v0Child, &v0ChildCount);
        int v0 = build(v0Child, v0ChildCount, i+1, v0Literal);
        if(v0Child)
            freeSetOfCubes(v0Child, v0ChildCount);

        // this allocs more space, but that's ok
        t_blif_cube **v1Child = (cubeCount==0)?(NULL):
            ((t_blif_cube **) malloc(cubeCount * sizeof(t_blif_cube *))); 
        int v1ChildCount = 0;
            int v1Literal = Cofactor(setOfCubes, cubeCount, i, LITERAL_1,
                            v1Child, &v1ChildCount);
			int v1 = build(v1Child, v1ChildCount, i+1, v1Literal);

        if(v1Child)
            freeSetOfCubes(v1Child, v1ChildCount);

       // printf("v0Literal=%d \t v1Literal=%d\n", v0Literal, v1Literal);
        int varIdx = curFunc->inputs[i]->data.index;
       // printf("varIdx = %d\n", varIdx);
        return MK(varIdx, v0, v1);
    }
} 

//---------------------------------------------------------------------
// create Cofactor table 
//---------------------------------------------------------------------
int Cofactor(t_blif_cube **oldCubes, int oldCubeCount, int idx, int val,
	t_blif_cube **newCubes, int *newCubeCount)
{

	int i;
	int cnt = 0;
	// find out num of new cubes
	for (i = 0; i < oldCubeCount; i++)
	{
		if ((read_cube_variable(oldCubes[i]->signal_status, idx) == val) ||
			(read_cube_variable(oldCubes[i]->signal_status, idx) == LITERAL_DC))
		{
			cnt++;
		}
	}
	*newCubeCount = cnt;

	// allocate the newCubes
	int j = 0;
	for (i = 0; (i<oldCubeCount) && (j<cnt); i++)
	{
		if ((read_cube_variable(oldCubes[i]->signal_status, idx) == val) ||
			(read_cube_variable(oldCubes[i]->signal_status, idx) == LITERAL_DC))
		{
			newCubes[j] = (t_blif_cube *)malloc(sizeof(t_blif_cube));
			assert(newCubes[j]);
			memcpy(newCubes[j++], oldCubes[i], sizeof(t_blif_cube));
			//newCubes[j++][0] = oldCubes[i][0]; 
		}
	}
	assert(j == cnt);

	if (idx == InputCount - 1)
	{
		// after setting xi to val, if there are no more matching, then f=false
		if (cnt == 0)
		{
			//  printf("no matching cubes for x%d\n", idx);
			return LITERAL_0;
		}
		// after setting the last variable x_(n-1), if there are matching ones,
		// then f=true
		else
		{
			//     printf("assigning last input and still found a match\n");
			return LITERAL_1;
		}
	}

	return -1;
}

int res(int u, int j, int b) 
{
    if(T[u].var > j)
        return u;
    else if (T[u].var < j)
        return MK(T[u].var, res(T[u].low, j, b), res(T[u].high, j, b));
    else
    {
        if(LITERAL_0 == b)
            return res(T[u].low, j, b);
        else
            return res(T[u].high, j, b);
    }
}


//---------------------------------------------------------------------
// Finds the root node of a function
//---------------------------------------------------------------------
int findOutput(t_blif_logic_circuit *circuit, char* varName)
{
    int node = -1;
    int i;
    for(i=0; i<circuit->primary_output_count; i++)
    {
        if(0==strcmp(varName, circuit->primary_outputs[i]->data.name))
        {
            node = outRoot[i];
            break;
        }
    }
    if(-1==node)
    {
        printf("ERROR: function %s not recognized!\n", varName);
    }

    return node;
}



// Remove the idx-th entry from the T table
void removeEntry(int idx, int newIdx) {
	int i;
	for (i = 2; i < TRows; i++) {
		if (T[i].low == idx) {
			assert(i != newIdx);
			T[i].low = newIdx;
		}
		if (T[i].high == idx) {
			assert(i != newIdx);
			T[i].high = newIdx;
		}
	}
	return;
}

// Simplify the entries in the T table

void simplify () {
    int i, j;
    // 1)
    for (i = 2; i < TRows; i++) {
        if (T[i].low < 0 || T[i].low > TRows || T[i].high < 0 || T[i].high > TRows) continue;
        if (T[i].low == T[i].high && T[i].low >= 0) {
            //printf("Redundant entry: %d\t|%d|%d|%d|\n", i, T[i].var, T[i].low, T[i].high);
            removeEntry(i, T[i].low);

            // Set low and high entries to -1 to signify that it is invalid
            T[i].low = -1;
            T[i].high = -1;
        }
    }

    // 2)
    for (i = 2; i < TRows; i++) {
        for (j = i + 1; j < TRows; j++) {
            if ((T[i].var == T[j].var) && (T[i].low == T[j].low) && (T[i].high == T[j].high)) {
                removeEntry(j, i);
                T[j].low = -1;
                T[j].high = -1;
            }
        }
    }
    return;
}


//Find order of variables
int findOrder (int *varOrder, int idx, int size) {
    int i;
    for (i = 0; i < size; i++) {
        if (varOrder[i] == idx) return i;
    }
    printf("Error, out of range!\n");
    assert(0);
}


// Resets all the T[i].swapped members to false 
void resetSwapped(TRow *T) {
    int i;
    for (i = 0; i < TRows; i++) {
        T[i].swapped = false;
    }
}


int RootNode (TRow *T, int* varOrder) {
    int i, j;
    for (i = 0; i < InputCount; i++) {
        for (j = 0; j < TRows; j++) {
            if (T[j].var == varOrder[i]) return j;
        }
    }
    assert(0);
}


int getPos(int *array, int val, int size) {
    int i;
    for (i = 0; i < size; i++) {
        if (array[i] > val) break;
    }
    return i;
}

// cleanup the T Table
void cleanUp() {
    int i;
    TRow *newT = (TRow*) malloc (INIT_SIZE * sizeof(TRow));
    // initialize value for the terminal nodes
    newT[0].var = TotalNodes; // terminal node 0
    newT[0].low = -1;
    newT[0].high = -1;
    newT[1].var = TotalNodes; // terminal node 1
    newT[1].low = -1;
    newT[1].high = -1;
    int newNumTRows = 2;
    int numInvalidRows = TRows - numValidRows(T);

    int *invalidRows = (int *) malloc (numInvalidRows * sizeof(int));

    int numInvalid = 0;
    for (i = 2; i < TRows; i++) {
               if (T[i].low == -1 || T[i].high == -1) {
            invalidRows[numInvalid++] = i; // invalid entry
        }
    }

    for (i = 2; i < TRows; i++) {
        if (T[i].low != -1 && T[i].high != -1) {
            newT[newNumTRows].var = T[i].var;
            newT[newNumTRows].low = T[i].low - getPos(invalidRows, T[i].low, numInvalid);
            newT[newNumTRows].high = T[i].high - getPos(invalidRows, T[i].high, numInvalid);
            newT[newNumTRows].swapped = false;
            newNumTRows++;
        }

    }

    free(T);
    T = newT;
    TRows = newNumTRows;
    return;
}

// Swap the variables inputs[var1] and inputs[var2] order in the T table

int swap(t_blif_cubical_function *f, int var1, int var2, TRow *T) 
{
    resetSwapped(T);
 //   printf("SWAP: %d <=> %d\n",var1, var2);

    int i;
    int table[4] = {-1, -1, -1, -1};
      for (i = 0; i < TRows; i++) {
        if (T[i].var == var1 && !T[i].swapped && T[i].low >= 0 
			&& T[i].low < TRows && T[i].high >= 0 && T[i].high < TRows)
		{
			if (T[i].low < TRows && T[i].low >= 0)
			{
                if (T[T[i].low].var == var2 && T[i].low > 1) { // a
                    table[0] = T[T[i].low].low;
                    table[1] = T[T[i].low].high;
                } else {
                    table[0] = T[i].low;
                    table[1] = T[i].low;
                }
            }

            if (T[i].high < TRows && T[i].high >= 0) {
                if (T[T[i].high].var == var2 && T[i].high > 1) { // a
                    table[2] = T[T[i].high].low;
                    table[3] = T[T[i].high].high;
                } else {
                    table[2] = T[i].high;
                    table[3] = T[i].high;
                }
            }

			if (T[i].low < TRows && T[i].low >= 0) {
                if (!(T[T[i].low].var == var2 && T[i].low > 1)) { // b, c
                    T[i].low = add(var1, table[0], table[2]);
                    T[T[i].low].swapped = true;
                }
            }

            if (T[i].high < TRows && T[i].high >= 0) {
                if (!(T[T[i].high].var == var2 && T[i].low > 1)) { // b, c
                    T[i].high = add(var1, table[1], table[3]);
                    T[T[i].high].swapped = true;
                }
            }

            // Swap the orders
            T[i].var = var2;
            T[i].swapped = true;
		    if (T[i].low < TRows && T[i].low > 1 && T[T[i].low].var == var2) {
		        T[T[i].low].var = var1;
		        T[T[i].low].high = table[2];
		        T[T[i].low].swapped = true;
		    }
		
		    if (T[i].high < TRows && T[i].high > 1 && T[T[i].high].var == var2) {
		        T[T[i].high].var = var1;
		        T[T[i].high].low = table[1];
		        T[T[i].high].swapped = true;
		    }

            simplify();
            cleanUp();

        }
    }
	return numValidRows(T);
}

//---------------------------------------------------------------------
// Sifting function
//---------------------------------------------------------------------
int sift(t_blif_cubical_function *f, t_blif_logic_circuit *circuit, int index)
{
    
    int i;

    TRow *copyT = (TRow *) malloc (INIT_SIZE * sizeof(TRow));
    memcpy(copyT, T, TRows * sizeof(TRow));

    int *varOrder = (int *) malloc (InputCount * sizeof(int));
    int *copyVarOrder = (int *) malloc (InputCount * sizeof(int));

	// initial order
    for (i = 0; i < InputCount; i++) {
        varOrder[i] = f->inputs[i]->data.index; 
    }
	printf("Sifting operation starts:\n");
    int copyNumTRows;
    int targetVar, varJidx;
    int version = 0;
	int count = 0;
    for (i = 0; i < InputCount; i++) {
        copyNumTRows = TRows;
        int minRows = numValidRows(T);
         memcpy(copyVarOrder, varOrder, InputCount * sizeof(int));
     // printVarOrder(varOrder, InputCount);
	  targetVar = findOrder(varOrder, i, InputCount);
	//  printf("Find optimal position for variable %s\n", circuit->primary_inputs[i]->data.name);
	//  Forward swap    
	//  printf("Forward swap\n");
	    bool reverse = (targetVar == 0) ? false : true; 
	    int optPos = targetVar;
        for (varJidx = targetVar + 1; varJidx < InputCount; varJidx++) {
            int tmpTRows = swap(f, varOrder[targetVar], varOrder[varJidx], T);
           
			int tmpT = build(f->set_of_cubes, f->cube_count, 0, f->value);
			count++;
            // update varOrder array
            int tmp = varOrder[targetVar];
            varOrder[targetVar] = varOrder[varJidx];
            varOrder[varJidx] = tmp;
            targetVar++;
             if (tmpTRows < minRows) {
                minRows = tmpTRows;
                optPos = targetVar;
            }
         
			 if (BuildBDD) printf("Version %d - ", version);
            if (BuildBDD) printVarOrder(varOrder, InputCount);
			
        }

        // Restore original T table
        TRows = copyNumTRows;
        memcpy(T, copyT, TRows * sizeof(TRow));
        memcpy(varOrder, copyVarOrder, InputCount * sizeof(int));
        targetVar = findOrder(varOrder, i, InputCount);

        // Backward swap
		
        if (reverse) {
		//	printf("backward swap\n");
            for (varJidx=targetVar - 1; varJidx >= 0; varJidx--) { 
                int tmpTRows = swap(f, varOrder[varJidx], varOrder[targetVar], T);
	//			int tmpT = build(f->set_of_cubes, f->cube_count, 0, f->value);
				count++;
                // update varOrder array
                int tmp = varOrder[targetVar];
                varOrder[targetVar] = varOrder[varJidx];
                varOrder[varJidx] = tmp;
                targetVar--;

	             if (tmpTRows < minRows) {
                    minRows = tmpTRows;
                    optPos = targetVar;
                }
            }
        }

        // Restore original T table
        TRows = copyNumTRows;
        memcpy(T, copyT, TRows * sizeof(TRow));
        memcpy(varOrder, copyVarOrder, InputCount * sizeof(int));
        targetVar = findOrder(varOrder, i, InputCount);
//  Move variable i to optimal position
        if (optPos < findOrder(varOrder, i, InputCount)) {
			for (varJidx=targetVar-1; varJidx >= optPos; varJidx--) { 
				// Start with var i at pos end
                swap(f, varOrder[varJidx], varOrder[targetVar], T);
                // update varOrder array
                int tmp = varOrder[targetVar];
                varOrder[targetVar] = varOrder[varJidx];
                varOrder[varJidx] = tmp;
                targetVar--;
                //printTTable(T, f);
				count++;
            }
        } else if (optPos > findOrder(varOrder, i, InputCount)) {
            for (varJidx=targetVar+1; varJidx <= optPos; varJidx++) { 
				// Start with var i at pos end
                swap(f, varOrder[targetVar], varOrder[varJidx], T);
                // update varOrder array
                int tmp = varOrder[targetVar];
                varOrder[targetVar] = varOrder[varJidx];
                varOrder[varJidx] = tmp;
                targetVar++;
                //printTTable(T, f);
				count++;
            }

        }
        memcpy(copyT, T, TRows * sizeof(TRow));
}
	
    simplify();
	printf("Total Swap Operation = %d \n\n", count);
	printf("\n");
	printf("Post Sifting Table\n");
	printTTable(T, f);
	free(copyT);
	free(copyVarOrder);
    return RootNode(T, varOrder);
}



/**********************************************************************/
/*** MAIN FUNCTION ****************************************************/
/**********************************************************************/
int main(int argc, char* argv[])
{
	printf("*********************************************\n");
	printf("ECE 1733 Assignment 2\nZeming Li and Yuchen Wang\n");
	printf("*********************************************\n");
	bool ApplyOperation = false;

	BuildBDD = false;
	t_blif_logic_circuit *circuit = NULL;

	bool flag = false;

	if (argc < 2)
	{
		printf("Usage: %s <source BLIF file> <options>\r\n", argv[0]);
		system("pause");
		return 0;
	}

	/* Read BLIF circuit. */
	printf("Reading blif file %s...\n",argv[1]);
	circuit = ReadBLIFCircuit(argv[1]);
    SiftOperation = 0;

	char* mod;
	mod = argv[2];

    switch((int)mod[0])
        {
            case 'a':
                ApplyOperation = true;
                break;
            case 's':
                SiftOperation = true;
                break;
            case 'b':
                BuildBDD = true;
                break;
         default:
                fprintf(stderr, "Usage: %s %s %s\n", argv[0], argv[1], argv[2]);
                fprintf(stderr, "\t-a\tEnables Apply operation\n");
                fprintf(stderr, "\t-s\tEnables sifting\n");
                fprintf(stderr, "\t-d\tEnables debug mode\n");
			
				system("pause");
                exit(EXIT_FAILURE);
        }

	if (circuit != NULL)
	{
		int index;
	/* build BDD for each function, one at a time. */
		printf("\n");

        TotalNodes = circuit->primary_input_count;
        initT();
        initG();
        outRoot = (int *) malloc(circuit->primary_output_count * sizeof(int));

		for (index = 0; index < circuit->function_count; index++)
		{
			t_blif_cubical_function *function = circuit->list_of_functions[index];
            curFunc = function;
			int i;
			printf("Function %s has inputs:\n", circuit->primary_outputs[index]->data.name);

            for(i=0; i<function->input_count; i++)
            {
                int idx = function->inputs[i]->data.index;
                printf("Input %s\n", circuit->primary_inputs[idx]->data.name);
            }
			printf("\n");

			InputCount = function->input_count;

            int tmp;

// ============================
// Build BDD
// ============================
	// TRows = 2;
     outRoot[index] = build(function->set_of_cubes, function->cube_count, 0, function->value);
	 //printf("outRoot %d\n\n",outRoot[index]);
	 if (!flag){
		 row1 = printTTable(T, function);
		 flag = true;
		 printf("Original BDD for Function %s.....\n", circuit->primary_outputs[index]->data.name);
		 printDOTfromNode(circuit->primary_inputs, outRoot[index], circuit->primary_outputs[index]->data.name, 0);
	 }
	 else
	 {
		printTTable2(T, function);
		printf("Original BDD for Function %s.....\n", circuit->primary_outputs[index]->data.name);
		printDOTfromNode2(circuit->primary_inputs, outRoot[index], circuit->primary_outputs[index]->data.name, 0);
	 }
// Sift Operation		
	 if (SiftOperation) {
		 int presift = TRows;
		 // get ticks per second
		 QueryPerformanceFrequency(&frequency);
		 // start timer
		 QueryPerformanceCounter(&t1);
		 tmp = sift(function, circuit, index);
		 QueryPerformanceCounter(&t2);
		 // compute and print the elapsed time in millisec
		 elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
		 rNodeIdx = tmp;
		 printf("BDD after sifting operation.....\n");
		 printDOTfromNode(circuit->primary_inputs, rNodeIdx,
						circuit->primary_outputs[index]->data.name, 0);
		 printf("Time taken for sifting operation = %f ms\n", elapsedTime);
		}
		}
	
//=====================================================
 //  perform Apply ops
		if (ApplyOperation)
			appMain(T, circuit, index);
//=====================================================
// clean up
		free(T);
		free(G);
        free(outRoot);
		printf("Done.\r\n");
		DeleteBLIFCircuit(blif_circuit);
	}
	else
	{
		printf("Error reading BLIF file. Terminating.\n");
	}
	system("pause");
	return 0;

}

