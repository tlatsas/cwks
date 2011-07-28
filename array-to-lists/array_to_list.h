/**********************************************************************
 * ArrayToList                                                        *
 * C Header File - ArrayToList.h                                      *
 *                                                                    *
 * Tasos Latsas                                                       *
 **********************************************************************/

#ifndef ARRAY_TO_LIST_H
#define ARRAY_TO_LIST_H

//------------------------------------------------------------------------------
// Include Files
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "single_list.h"

//------------------------------------------------------------------------------
// Constants Declaration
//------------------------------------------------------------------------------
#define MAX_ROW      10
#define MAX_COLUMN   10

//------------------------------------------------------------------------------
// ArrayToList Function Protoypes
//------------------------------------------------------------------------------
int MenuSelection();
void DisplayArray(int** iArray, int iRow, int iColumn);
void DeleteDynArray(int** iArray, int iRow);
int** Allocate_2D_Array(int iRow, int iColumn);

#endif
