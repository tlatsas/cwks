/*****************************************
 * ArrayToList                           *
 * C Source File - ArrayToList.c         *
 *                                       *
 * Tasos Latsas                          *
 *****************************************/

//------------------------------------------------------------------------------
// Include Files
//------------------------------------------------------------------------------
#include "array_to_list.h"

//------------------------------------------------------------------------------
// Main Program
//------------------------------------------------------------------------------
int main()
{   
     // Table dimensions variables
     int iRow, iColumn;

     // Get dimensions
     while(1)
     {         
         system("cls"); // Clear Screen
         printf("Give array dimensions.\nRows:");
         scanf("%d", &iRow);
         printf("Columns:");
         scanf("%d", &iColumn);
           
         if ((iRow > 1 && iRow <= MAX_ROW) && (iColumn > 1 && iColumn <= MAX_COLUMN))
            break;
     }

     // Create the array
     int** iArray = Allocate_2D_Array(iRow, iColumn);

     // Create list's head pointer
     struct node* pHead = NULL;
     
     // Create list's tail pointer
     struct node* pTail = NULL;
     
     // Create the first node which contains the array's dimensions
     // pHead now points to that node
     if (! AddNode(&pHead, iRow, iColumn, 0))
        exit(1);
     
     // Get array values     
     int i, j;
     
     for (i = 0; i < iRow; i++)
     {
         for (j = 0; j < iColumn; j++)
         {
             printf("Give the [%d][%d] element:", i, j);
             scanf("%d", &iArray[i][j]);
             // If not a zero value then add node
             if (iArray[i][j] != 0)
             {
                // Locate the tail node and store its address in pTail
                LocateTail(pHead, &pTail);
                
                // Add new node after the last node (tail node)
                if (! AddNode(&pTail->m_pNext, i, j, iArray[i][j]))
                   exit(1);
             }
         }
         printf("\n");
     }
     
     // Show msg
     printf("Single Linked List created successfully\n\n");
         
     int quit = FALSE;
     
     while (quit == FALSE)
     {
         printf("Press any key to continue...");
         getch();
         switch(MenuSelection())
         {
             case 1:
             {
                  DisplayArray(iArray, iRow, iColumn);
                  break;
             }
             case 2:
             {
                  DisplayList(pHead);
                  break;
             }
             case 3:
             {

                  int** iArrayTwo = Allocate_2D_Array(pHead->m_iX_Pos, pHead->m_iY_Pos);
                  
                  for (i = 0; i < pHead->m_iX_Pos; i++)
                      for (j = 0; j < pHead->m_iY_Pos; j++)
                          iArrayTwo[i][j] = 0;
         
                  struct node* pCurrent = pHead;                  
         
                  while (pCurrent != NULL)
                  {
                       if (pCurrent->m_iData != 0)
                           iArrayTwo[pCurrent->m_iX_Pos][pCurrent->m_iY_Pos] = pCurrent->m_iData;
                       pCurrent = pCurrent->m_pNext;
                  }
                  
                  printf("New Array created successfully: \n\n");
                  DisplayArray(iArrayTwo, pHead->m_iX_Pos, pHead->m_iY_Pos);
                  
                  printf("\nThe program will now exit\n\n");
                  DeleteDynArray(iArrayTwo, pHead->m_iX_Pos);
                  
                  quit = TRUE;
                  break;
             }
             case 4:
             {
                  quit = TRUE;
                  break;
             }
         }
     }

     // Free dynamic allocated memory - 2D Arrays + List
     DeleteDynArray(iArray, iRow);
     DeleteList(pHead);
     
     system("pause");
     return 0;
}


//------------------------------------------------------------------------------
// ArrayToList Function Source
//------------------------------------------------------------------------------
int MenuSelection() 
// Displays main menu, returns user's selection
{                                                     
    int selection;
    
    system("cls");
    printf("******************** MAIN MENU ***********************\n");
    printf("*                                                    *\n");
    printf("* 1 --> Display Array                                *\n");
    printf("* 2 --> Display Linked List                          *\n");
    printf("* 3 --> Reverse procedure                            *\n");
    printf("* 4 --> Quit                                         *\n");
    printf("*                                                    *\n");
    printf("******************************************************\n\n");
    
    do
    {
        printf("Enter your choice >");
        scanf("%d", &selection);
    } while (selection<1 || selection>4);
    printf("\n");
    
    return selection;
}


void DisplayArray(int** iArray, int iRow, int iColumn)
{
     int i, j;
     
     for (i = 0; i < iRow; i++)
     {
         for (j = 0; j < iColumn; j++)
             printf("[%d][%d]:%4d   ", i, j, iArray[i][j]);
         printf("\n");
     }
}


void DeleteDynArray(int** iArray, int iRow)
{
     int i;
     
     // Delete subarrays first
     for (i = 0; i < iRow; i++)
         free(iArray[i]);
     // Delete main pointer array
     free(iArray);
}


int** Allocate_2D_Array(int iRow, int iColumn)
{
      // Create a dynamic array
        // First allocate an array to pointers-to-int
      int** iNewArray = malloc( iRow * sizeof(int*));

        // If not enough memory then exit
      if (! iNewArray)
         exit(1);
      
      int i;
      
        // Create subarrays      
      for (i = 0; i < iRow; i++)
      {
          iNewArray[i] = malloc( iColumn * sizeof(int));
          if (! iNewArray[i]) 
             exit(1);
      }
          
      return iNewArray;
}
