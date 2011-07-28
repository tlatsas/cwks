/*******************************************************************************
 * Single Linked List Function Code                                            *
 * C Source File - SingleList.c                                                *
 *                                                                             *
 * Tasos Latsas                                                                *
 *******************************************************************************/

//------------------------------------------------------------------------------
// Include Files
//------------------------------------------------------------------------------
#include "single_list.h"

//------------------------------------------------------------------------------
// Function Source
//------------------------------------------------------------------------------
int AddNode(struct node** pPosition, int iX_Pos, int iY_Pos, int iData)
{
     // Create a new node
     struct node* pNode = malloc(sizeof(struct node));
     
     // Check if memory allocated successfully
     if (!pNode)
        return FALSE;
     
     // Enter data fields
     pNode->m_iX_Pos = iX_Pos;
     pNode->m_iY_Pos = iY_Pos;
     pNode->m_iData  = iData;
     
     // Link node to list
     pNode->m_pNext  = *pPosition;
     *pPosition      = pNode;
     
     return TRUE;
}


void DisplayList(struct node* pHead)
{
   struct node* pCurrent = pHead;
   int iNodeCounter = 0;
   
   while (pCurrent != NULL)
   {
      iNodeCounter++;
      if (iNodeCounter != 1)
      {
         printf("Node n%d   -> Position in array: [%d][%d]   Data: %d\n", iNodeCounter,
                                                                          pCurrent->m_iX_Pos,
                                                                          pCurrent->m_iY_Pos,
                                                                          pCurrent->m_iData);
      }
      else
      // if this is the first node handle print differently
      {
          printf("Info node -> Array size: %d x %d           Data: %d\n", pCurrent->m_iX_Pos,
                                                                          pCurrent->m_iY_Pos,
                                                                          pCurrent->m_iData);
      }
      // Proceed to next node
      pCurrent = pCurrent->m_pNext;
   }
}


void LocateTail(struct node* pHead, struct node** pTail)
// Locates the tail node, returns its address in pTail
{
   struct node* pCurrent = pHead;
   
   while (pCurrent->m_pNext != NULL)
   {
      pCurrent = pCurrent->m_pNext;
   }   
   
   *pTail = pCurrent;  // *pTail contains the address of the tail node
}


void DeleteList(struct node* pHead)
{
     struct node* pTmp     = NULL;
     struct node* pCurrent = pHead;
     
     while (pCurrent != NULL)
     {
           pTmp = pCurrent->m_pNext;
           free(pCurrent);
           pCurrent = pTmp;
     }
     
}
