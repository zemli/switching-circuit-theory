
#include "cubical_function_representation.h"
#include <windows.h>

int appOp(TRow *T, int op, int u1, int u2);
void appMain(TRow *T, t_blif_logic_circuit *circuit, int bddNum);
int perfOp(int op, int u1, int u2);
int translateOp(char* op);
