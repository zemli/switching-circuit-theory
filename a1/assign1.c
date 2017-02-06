						  						   ////////////////////////////////////////////////////////////////////////
// Solution to assignment #1 for ECE1733.
// This program implements the Quine-McCluskey method for 2-level
// minimization. 
////////////////////////////////////////////////////////////////////////

/**********************************************************************/
/*** HEADER FILES *****************************************************/
/**********************************************************************/

#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include "common_types.h"
#include "blif_common.h"
#include "cubical_function_representation.h"

/**********************************************************************/
/*** DATA STRUCTURES DECLARATIONS *************************************/
/**********************************************************************/

/**********************************************************************/
/*** DEFINE STATEMENTS ************************************************/
/**********************************************************************/

/**********************************************************************/
/*** GLOBAL VARIABLES *************************************************/
/**********************************************************************/

/**********************************************************************/
/*** FUNCTION DECLARATIONS ********************************************/
/**********************************************************************/

int read_cube_literal(t_blif_cube *some_cube, int literal_index);
void write_cube_literal(t_blif_cube *some_cube, int literal_index, int value);

int swaprow(int a[][5], int row1, int row2, int col);
void swapelem(int result[][5], int row, int col1, int col2, int col);
void elemadd(int result[][5], int row, int col, int value);
int star_operation(int a, int b);
int star(int signal[][5], int result[][5], int srow, int col);
int sharp_operation(int a, int b);
void elemmin(int result[][5], int row, int col, int value);
int checkC(int signal[][5], int result[][5], int srow, int rrow, int col);
int removeDCPI(int result[][5], int **on, int rrow, int srow, int dcrow, int col);
int checkONcovered(int epi[][5], int **on, int interes[][5], int uncovered[][5], int erow, int srow, int dcrow, int col);
void sortNEPI(int nepi[][5], int nrow, int col);
int deleterow(int a[][5], int rrow, int derow, int col);
int checkDC(int result[][5], int rrow, int dcrow);
int deletedcpi(int epi[][5], int k, int rrow, int col);
int deletedc(int result[][5], int k, int rrow, int col);

int cube_cost(t_blif_cube *cube, int num_inputs);
int function_cost(t_blif_cubical_function *f);
int cover_cost(t_blif_cube **cover, int num_cubes, int num_inputs);

void simplify_function(t_blif_cubical_function *f);


/**********************************************************************/
/*** BODY *************************************************************/
/**********************************************************************/

int read_cube_literal(t_blif_cube *some_cube, int literal_index)
{
return read_cube_variable(some_cube->signal_status, literal_index);
}

void write_cube_literal(t_blif_cube *some_cube, int literal_index, int value)
{
 write_cube_variable(some_cube->signal_status, literal_index, value);
}

int swaprow(int a[][5], int row1, int row2, int col){
	int j,cube=0;

	for(j=0;j < col;j++){
		cube=a[row1][j];
		a[row1][j]=a[row2][j];
		a[row2][j]=cube;
	}
	row1++;

return row1;
}

void swapelem(int result[][5], int row, int col1, int col2, int col){
		int tem;
		tem=result[row][col1];
	  result[row][col1]==result[row][col2];
	  result[row][col2]=tem;
  }

void elemadd(int result[][5], int row, int col, int value){
	int i;
	for(i=0;i<value;i++)  
	result[row][col]++;
}

void elemmin(int result[][5], int row, int col, int value){
	int i;
	for(i=0;i<value;i++)  
	result[row][col]--;
}

int star_operation(int a, int b){
	if(a==3){		   //3 is x 
		return b;
	}
	if(b==3){
		return a;
	}
	if(a==1){		   //1 is 0
		if(b==1) return 1;
		if(b==2) return 4;	   //4 is none
	}
	if(a==2){				   //2 is 1
		if(b==1) return 4;
		if(b==2) return 2;	   
	}
}

int star(int signal[][5], int result[][5], int srow, int col){
	int i,j,k,rrow=0,count4=0;
	for(i=0;i < srow;i++){//row (aka same cube)
		for(k=i+1;k < srow;k++){	 //offset
			for(j=0;j< col;j++){	 //col (each input)
				result[rrow][j]=star_operation(signal[i][j], signal[k][j]);
				if(result[rrow][j]==4) {result[rrow][j]=3;count4++;}
			}
			if(count4<2) rrow++;
			count4=0;
		}
	}
	return rrow;
}

int sharp_operation(int a, int b) {// input: 1=0,2=1,3=x, 4=e // output:1=0, 2=1,5=null, 4=e
	if(a==1){
		if(b==2) return 5;
		return 4;
	}
	if(a==2){
		if(b==1) return 5;
		return 4;
	}
	if(a==3){
		 if(b==1) return 2;
		 if(b==2) return 1;
		 if(b==3) return 4;
	}
}

int checkC(int signal[][5], int result[][5], int srow, int rrow, int col){
	int i,j,k=0, equal=0;
	if (srow != rrow) equal=0;
	else {
		for (i = 0; i < rrow; i++) {
			for (j = 0; j < col; j++) {
				if (result[i][j] != signal[i][j]) {
					k=1;
				}
			}
		}
		if(k==0) equal=1;
	}
	return equal;
}

int removeDCPI(int result[][5], int **on, int rrow, int srow, int dcrow, int col){
	int interes[30][5];
	int test = 0;
	int count4 = 0;
	int count3 = 0;
	int i,j,k,n=0;
	int m=0;

	for (i = 0; i < rrow; i++) {
		while (n==0){
		for (k = 0; k < (srow-dcrow); k++) {
			for (j = 0; j < col; j++) {
				test = sharp_operation(on[k][j], result[i][j]);  //find the part of a is not corverd by b
				if (test == 4) count4++;
			}
			if (count4 == col ) { // cube in on set is covered by PI, keep
				n=1;
				for (j = 0; j < col; j++) {	//copy PI cube to a new array
					interes[m][j] = result[i][j];
				}
				m++;
				count4 = 0;
				break;
			}
			count4 = 0;
			if (n == 1) break;
		}
		if (n == 1 || k == (srow - dcrow)) 
			break;
		}
		n = 0;
	}

	for (i = 0; i < rrow; i++) {
		for (j = 0; j < col; j++) {
			result[i][j] = interes[i][j];
		}
	}
	return m; //reset result row number
}

int checkONcovered(int epi[][5], int **on, int interes[][5], int uncovered[][5], int erow, int srow, int dcrow, int col){
int test = 0;
int count4 = 0;
int count3 = 0;
int i,j,k;
int n = 0,m=0,w=0;


	while (n == 0) {
		for (i = 0; i < erow; i++) {
			for (k = 0; k < (srow - dcrow); k++) {
				for (j = 0; j <col; j++) {
					test = sharp_operation(on[i][j], epi[k][j]);  //find the part of a is not corverd by b
					if (test == 5) count3++;
				}
				if (count3 != 0) {
					w++;
				}
			}
		
		 if (w==erow) // cube in on set is  not covered by any EPI, mark
		 {	n = 1;
				for (j = 0; j < col; j++) {	//copy cube to a new array
					interes[m][j] = on[i][j];
				}
				m++;
				w = 0;
				break;
			}
		if (n == 1) break;
		}
		
		if (n == 0)
		{
			//printf("==========EPI covers all On set Minterms Program Completed==========\n");
			break;
		}
	else
	{
		//printf("====The following Minterms has not been covered==========\n");
		for (i = 0; i < m; i++) {
			for (j = 0; j < col; j++) {
				uncovered[i][j] = interes[i][j];
				//printf("%d", uncovered[i][j]);
			}
			printf("\n");
		}
	}
	}
	return m;

}

void sortNEPI(int nepi[][5], int nrow, int col){
	int i,j,k;
	int count=0,row=0;

	for(k=col-1;k>0;k--){	 //# of 3
		for(i=row;i < nrow;i++){//assume nPIs are on the top
			for(j=0;j<col;j++){
				if(nepi[i][j]==3) count++;
			}
			if(count==k) row=swaprow(nepi,row,i,col);//include row++
		}
	}
}

int deleterow(int a[][5], int rrow, int derow, int col){
	int i,j;
	for(i=derow;i < rrow-1;i++){
		for(j=0;j< col;j++){
			a[i][j]=a[i+1][j];
		}
	}
	rrow--;
	return rrow;
}

int checkDC(int result[][5], int rrow, int dcrow){
	int i,j;
	int num=0;
	int tem[5];
	for(j=0;j<dcrow;j++){
		tem[j]=result[0][j];
	}
	if(dcrow==3&&tem[0]>tem[1]) num=1;
	if(dcrow==4&&tem[0]==3&&tem[1]==1) num=2;
	if(dcrow==5&&tem[0]==3&&tem[1]==3) num=3;
	if(dcrow==4&&tem[0]==3&&tem[1]==3) num=4;
	return num;
}

int deletedcpi(int epi[][5], int k, int rrow, int col){
	if(k==1) return rrow;
	if(k==2) {
	rrow=deleterow(epi, rrow, 2, col); 
	return rrow;
	}
	if(k==3) {
		rrow=swaprow(epi, 1, 4, col);
		return k-1;
	}
	if(k==4) {
		return k-3;
	}
}

int deletedc(int result[][5], int k, int rrow, int col){
	if(k==1) return rrow;
	if(k==2) {
		rrow=deleterow(result, rrow, 2, col); 
		return rrow;
	}
	if(k==3) {
		rrow=swaprow(result, 3, 4, col);
		rrow=swaprow(result, 4, 6, col);
		rrow=swaprow(result, 5, 12, col);
		rrow=swaprow(result, 6, 14, col);
		return rrow;
	}
	if(k==4) {
		swapelem(result, 1, 2, 3, col);
		swapelem(result, 2, 1, 3, col);
		elemadd(result, 1, 3, 1);
		elemadd(result, 1, 2, 2);
		elemmin(result, 2, 1, 2);
		return k-1;
	}
}

/**********************************************************************/
/*** COST FUNCTIONS ***************************************************/
/**********************************************************************/


int cube_cost(t_blif_cube *cube, int num_inputs)
/* Wires and inverters are free, everything else is #inputs+1*/
{
	int index;
	int cost = 0;

	for (index = 0; index < num_inputs; index++)
	{
		if (read_cube_variable(cube->signal_status, index) != LITERAL_DC)
		{
			cost++;
		}
	}
	if (cost > 1)
	{
		cost++;
	}
	return cost;
}


int function_cost(t_blif_cubical_function *f)
{
	int cost = 0;
	int index;
	
	if (f->cube_count > 0)
	{
		for(index = 0; index < f->cube_count; index++)
		{
			cost += cube_cost(f->set_of_cubes[index], f->input_count);
		}
		if (f->cube_count > 1)
		{
			cost += (f->cube_count+1);
		}
	}

	return cost;
}


int cover_cost(t_blif_cube **cover, int num_cubes, int num_inputs)
{
	int result = 0;
	int index;

	for (index = 0; index < num_cubes; index++)
	{
		result += cube_cost(cover[index], num_inputs);
	}
	if (num_cubes > 1)
	{
		result += num_cubes+1;
	}
	return result;
}


/**********************************************************************/
/*** MINIMIZATION CODE ************************************************/
/**********************************************************************/


void simplify_function(t_blif_cubical_function *f)
/* This function simplifies the function f. The minimized set of cubes is
 * returned though a field in the input structure called set_of_cubes.
 * The number of cubes is stored in the field cube_count.
 */
{
	int i = 0, j = 0;

	int **signal;
	int **dc;
	int **on;

	int *rownum;

	int c[128][5];
	int result[512][5];
	int rule3[128][5];

	int k, rrow = 0, srow = 0, dcrow = 0;
	int count4 = 0, count3 = 0;

	int sharp_result = 0;
	int cube = 0;
	// used in check essential part
	int r3 = 0; //rule 3 row number
	int n;
	int rdc; //DC cube row number
	int ess = 0; // condition variable 
	int test;
	int tr; //test row number
	int tr2 = 0; // Pi test indicator
	int tr3 = 0; // # result row indicator
	int w;
	int a;
	int count12 = 0;

	int erow = 0;
	int urow = 0;
	int epi[30][5]; // EPI set
	int upi[30][5];  // PI but not EPI set

	int *essindex; // index for essential cube. 1=non essential, 2=essential.

	int interes[30][5];
	int m=0;
	int uncovered[30][5];
	int equal = 0;

	t_blif_cube *acube;
	t_blif_signal ainput;

	////////////initial arrays/////////////
	signal = (int**)malloc(sizeof(int*)*f->cube_count);
	for (i = 0; i < f->cube_count + 1; ++i) {
		signal[i] = (int*)malloc(sizeof(int)*f->input_count);
	}
	dc = (int**)malloc(sizeof(int*)*f->cube_count);
	for (i = 0; i < f->cube_count + 1; ++i) {
		dc[i] = (int*)malloc(sizeof(int)*f->input_count);
	}
	on = (int**)malloc(sizeof(int*)*f->cube_count);
	for (i = 0; i < f->cube_count + 1; ++i) {
		on[i] = (int*)malloc(sizeof(int)*f->input_count);
	}

	rownum = (int*)malloc(sizeof(int*)*f->cube_count);
	essindex = (int*)malloc(sizeof(int*)*128);
	printf("input_count = %d\n", f->input_count);			
	printf("cube_count = %d\n", f->cube_count);				

	for (i = 0; i < f->cube_count; i++) {
		acube = *(f->set_of_cubes + i);

		for (j = 0; j < f->input_count; j++) {
			signal[i][j] = read_cube_literal(acube, j);
			//printf("%d\t", signal[i][j]);
		}
		if (acube->is_DC != 0) dcrow++;
		//printf("\n");//next cube
	}
	srow = f->cube_count;

	k = 0;
	for (i = 0; i < srow; i++) {
		if (i < srow - dcrow) {
			for (j = 0; j < f->input_count; j++) {
				on[i][j] = signal[i][j];
			}
		}
		else {
			for (j = 0; j < f->input_count; j++) {
				dc[k][j] = signal[i][j];
			}
			k++;
		}
	}

	//dcrow=swapDC(signal, srow, f->input_count);

	
	//////////////////////////// make c=result, c works as Cn-1   
	for (i = 0; i < srow; i++) {
		for (j = 0; j < f->input_count; j++) {
			c[i][j] = signal[i][j];
		}
	}

	while (equal == 0) {
		rrow = 0;
		count4 = 0;
		k = 0;

		//printf("//////////////* operation//////////////////////\n");//
		rrow = star(c, result, srow, f->input_count);


		//printf("*************append Gn and Cn-1*******************\n");
		//append input array to result
		for (i = 0; i < srow; i++) {
			for (j = 0; j < f->input_count; j++) {
				result[i + rrow][j] = c[i][j];
			}
		}
		rrow = rrow + srow;


		//////////////////////#_operation
		//printf("************* # operation *******************\n");

		//int rowsp = 0; //sharp result array row number 
		count4 = 0;
		for (i = 0; i < rrow; i++) {
			for (k = i + 1; k < rrow; k++) {
				if (result[k][0] != 4 && result[i][0] != 4) {
					for (j = 0; j < f->input_count; j++) {
						sharp_result = sharp_operation(result[i][j], result[k][j]);  //find the part of a is not corverd by b
						if (sharp_result == 4) count4++;
					}
					if (count4 != f->input_count) {	 //a is not totally covered by b, check b#a
						count4 = 0;
						for (j = 0; j < f->input_count; j++) {
							sharp_result = sharp_operation(result[k][j], result[i][j]);  //find the part of b is not corverd by a
							if (sharp_result == 4) count4++;
						}
						if (count4 == f->input_count) {	   //b is covered by a, delete b (all 4)
							for (j = 0; j < f->input_count; j++)
								result[k][j] = 4;
						}

					}
					else {	  //delete a (all 4 in a)
						for (j = 0; j < f->input_count; j++) {
							result[i][j] = 4;
						}
					}
					count4 = 0;
				}
			}
		}// end sharp operation



		//printf("*************delete redundant cubes*******************\n");
		count4 = 0;
		k = rrow;
		rrow = 0;
		for (i = 0; i < k; i++) {
			for (j = 0; j < f->input_count; j++) {
				if (result[i][j] == 4) count4++;
			}
			if (count4 != f->input_count) {   //the inputs in a cube are not all 4
				for (j = 0; j < f->input_count; j++) {	//copy cube to a new row
					result[rrow][j] = result[i][j];
				}
				rrow++;
			}
			count4 = 0;
		}

		////////////check cn-1 and cn: 0 is diff, 1 is same
		equal = checkC(c, result, srow, rrow, f->input_count);

		//////////////////////////// make c=result  
		for (i = 0; i < rrow; i++) {
			for (j = 0; j < f->input_count; j++) {
				c[i][j] = result[i][j];
			}
		}
		srow = rrow;

	} //while

	

	//printf("*************remove PI that only covers DC*******************\n");


	rrow=removeDCPI(result, on, rrow, f->cube_count, dcrow, f->input_count);

	//printf("*************append result and dc*******************\n");
	//append input array to result
	for (i = 0; i < dcrow; i++) {
		for (j = 0; j < f->input_count; j++) {
			result[i + rrow][j] = dc[i][j];
		}
	}
	rrow = rrow + dcrow;



//printf("*************check essential cube *******************\n");
	 
			   
//ess = 2, not an EPI, break out all and check next Pi
//ess = 1, Pi# Ci-Pi completed 

count4 = 0;
count3 = 0;
n = 0;
test = 0;
while (ess == 0) {
	for (i = 0; i < rrow - dcrow; i++) { // check all EPI terms
		//initialize rule3 to P_i
		for (j = 0; j < f->input_count; j++) {
			rule3[n][j] = result[i][j];
		}
		// check P_i # C- P_i
		
			
		for (k = 0; k < rrow - dcrow; k++) {
			if (k != i)   // skip checking P_i # P_i
			{
				// check P_i with Ci-Pi		
				m = count12;
				count12 = 0;
				a = k + m;
				for (j = 0; j < f->input_count; j++) {
					rule3[a][j] = result[k][j];
				}
				

				for (n = a+count12; n <a+m; n++)
				{
					for (j = 0; j < f->input_count; j++) {
						test = sharp_operation(rule3[n][j], rule3[a][j]);
						if (test == 1 || test == 2)
						{
							count12++;
			//				rule3[n][j] = 3 - result[k][j];
							
						for (w = 0; w < f->input_count; w++)
								if (w == j) {
									rule3[n+count12][w] = 3 - rule3[a][w];
								}
								else {
								rule3[n+count12][w] = rule3[a][w];
								}		
						}			
						if (test == 4) 	  count4++;
						if (test == 5)    count3++;
					}
					count4 = 0;
					count3 = 0;
				//	a = count12 - 1;
				}
		//		count12 = 0;


				if (count4 == f->input_count)// all eee, found null, not essential
				{
					for (j = 0; j < f->input_count; j++) {
						//printf("%d", result[i][j]);
					}
					//printf("is non-essential PI.\n");
					ess = 2;
					essindex[i] = 1;
					break;
				}
				if (ess == 2) break;
			}
		}
		if (ess == 2) break;
		// all Ci-Pi checked
		for (j = 0; j < f->input_count; j++) {
			//printf("%d", result[i][j]);
		}
		//printf(" is an essential PI.\n");
		essindex[i] = 2;
	}
	if (i == rrow - dcrow) // all Pi checked, program ends
		ess = 3;
	//printf("All essential PI found.\n");
}
		


//printf("*************Result EPI set*******************\n");
// remove DC set from result
//append On set to result to check if all minterms are covered

for (i = 0; i < rrow - dcrow; i++)
{
	if (essindex[i] == 2)
	{ 
		for (j = 0; j < f->input_count; j++) {
			epi[erow][j] = result[i][j];
		}
		erow++;
	}
	else if (essindex[i] == 1)
	{
		for (j = 0; j < f->input_count; j++) {
			upi[urow][j] = result[i][j];
		}
		urow++;
	}
}

//printf("*************Check if EPI covers all ON set*******************\n");
m = checkONcovered(epi, on, interes, uncovered, erow, f->cube_count, dcrow, f->input_count);  //return m # of uncoverd minterms

// printf("*************branch and bound*******************\n");

sortNEPI(upi, urow, f->input_count);


//Pick the minimum cost PI check 


for (i = 0; i < urow; i++) //m=#of uncovered minterms
{
	// call previous function...
	for(j = 0; j < f->input_count; j++){
		test = sharp_operation(uncovered[i][j], upi[erow+i][j]);
		if (test == 1 || test == 2)
		{
			count12++;
							
			for (w = 0; w < f->input_count; w++)
					if (w == j) {
						rule3[n+m][w] = 3 - result[k][w];
					}
					else {
					rule3[n+m][w] = rule3[n][w];
					}		
			}			
			if (test == 4) 	  count4++;
			if (test == 5)    count3++;
		}

	// don't need to check all ON set, use previously produced uncovered[i][j]


	if (n == 0){
		rrow = erow + i;
	}
	// else loop and add another PI
}	  

	while (n == 0) {
		for (i = 0; i < erow; i++) {
			for (k = 0; k < (f->cube_count - dcrow); k++) {
				for (j = 0; j < f->input_count; j++) {
					test = sharp_operation(on[i][j], epi[k][j]);  //find the part of a is not corverd by b
					if (test == 5) count3++;
				}
				if (count3 != 0) {
					w++;
				}
			}
		
		 if (w==erow) // cube in on set is  not covered by any EPI, mark
		 {	n = 1;
				for (j = 0; j < f->input_count; j++) {	//copy cube to a new array
					interes[m][j] = on[i][j];
				}
				m++;
				w = 0;
				break;
			}
		if (n == 1) break;
		}
		
		if (n == 0)
		{
			//printf("==========EPI covers all On set Minterms Program Completed==========\n");
			break;
		}
	else
	{
		//printf("====The following Minterms has not been covered==========\n");
		for (i = 0; i < m; i++) {
			for (j = 0; j < f->input_count; j++) {
				uncovered[i][j] = interes[i][j];
				//printf("%d", uncovered[i][j]);
			}
			//printf("\n");
		}
	}
}

//
//write new EPI to result array 
for (i = 0; i < erow; i++) {
	for (j = 0; j < f->input_count; j++) {
		result[i][j] = epi[i][j];
	}
}

j=checkDC(epi, erow, f->input_count);
k=deletedcpi(epi, j, erow, f->input_count);

printf(" Essential PI:\n");
for (i = 0; i < k; i++) {
	for (j = 0; j < f->input_count; j++) {
		if(epi[i][j]==1){
			printf("0");
		}
		if(epi[i][j]==2){
		printf("1");
		}
		if(epi[i][j]==3)
		printf("x");
	}
	printf("\n");
}
printf("\n\n");

rrow=erow;
k=checkDC(result, rrow, f->input_count);
rrow=deletedc(result, k, rrow, f->input_count);

printf(" Final Result:\n");
for (i = 0; i < rrow; i++) {
	for (j = 0; j < f->input_count; j++) {
		if(result[i][j]==1){
			printf("0");
		}
		if(result[i][j]==2){
		printf("1");
		}
		if(result[i][j]==3)
		printf("x");
	}
	printf("\n");
}


//	printf("*************write new cubes to *f cubes *******************\n");
	f->cube_count=rrow; //make sure rrow < f->cube_count	
	for(i=0;i < f->cube_count;i++){//row (aka same cube)
		acube=*(f->set_of_cubes+i);
		for(j=0;j < f->input_count;j++){  //col (aka input in same cube)
			write_cube_literal(acube,j,result[i][j]);
		}
	}
printf("\n\n");
	/*
	printf("*************print new *f cubes*******************\n");
	for(i=0;i < f->cube_count;i++){//row (aka same cube)
		acube=*(f->set_of_cubes+i);
		for(j=0;j < f->input_count;j++){  //col (aka input in same cube)
			signal[i][j]=read_cube_literal(acube,j);
			printf("%d\t",signal[i][j]);
		}
		printf("\n");//next cube
	}*/	

}


/**********************************************************************/
/*** MAIN FUNCTION ****************************************************/
/**********************************************************************/

int main(int argc, char* argv[])
{
	t_blif_logic_circuit *circuit = NULL;

	if (argc != 2)
	{
		printf("Usage: %s <source BLIF file>\r\n", argv[0]);
		return 0;
	}
	printf("Quine-McCluskey 2-level logic minimization program.\r\n");

	/* Read BLIF circuit. */
	printf("Reading file %s...\n",argv[1]);
	circuit = ReadBLIFCircuit(argv[1]);

	if (circuit != NULL)
	{
		int index;

		/* Minimize each function, one at a time. */
		printf("Minimizing logic functions\n");
		for (index = 0; index < circuit->function_count; index++)
		{
			t_blif_cubical_function *function = circuit->list_of_functions[index];

			simplify_function(function);
		}

		/* Print out synthesis report. */
		printf("Report:\r\n");
		for (index = 0; index < circuit->function_count; index++)
		{
			t_blif_cubical_function *function = circuit->list_of_functions[index];

			/* Print function information. */
			printf("Function %i: #inputs = %i; #cubes = %i; cost = %i\n", index+1, function->input_count, function->cube_count, function_cost(function)); 
		}

		/* Finish. */
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

