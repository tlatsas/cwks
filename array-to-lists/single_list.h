/*******************************************************************************
 * Single Linked List Function Prototypes                                      *
 * C Header File - SingleList.h                                                *
 *                                                                             *
 * Tasos Latsas                                                                *
 *******************************************************************************/
 
#ifndef SINGLE_LIST_H
#define SINGLE_LIST_H

//------------------------------------------------------------------------------
// Include Files
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
// Constants Declaration
//------------------------------------------------------------------------------
#define FALSE      0
#define TRUE       1

//------------------------------------------------------------------------------
// Define Struct Node
//------------------------------------------------------------------------------
struct node
{
       int           m_iX_Pos;
       int           m_iY_Pos;
       int           m_iData;
       struct node*  m_pNext;
};

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
int AddNode(struct node** pPosition, int iX_Pos, int iY_Pos, int iData);
void DisplayList(struct node* pHead);
void LocateTail(struct node* pHead, struct node** pTail);
void DeleteList(struct node* pHead);

#endif
