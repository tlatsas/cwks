/*
****************************************
*  Author : Tasos Latsas               *
****************************************
*/

#include <stdio.h>
#include <stdlib.h>

struct node
   {
      int info;
      struct node* next;
   };

//==============================================================================

int menu_selection()                                  //displays main menu 
{                                                     //returns user's selection
    int selection;
    
    printf("******************** MAIN MENU ***********************\n");
    printf("*                                                    *\n");
    printf("* 1 --> Enter integer number at the end of the list  *\n");
    printf("* 2 --> Enter integer number at the head of the list *\n");
    printf("* 3 --> Search node                                  *\n");
    printf("* 4 --> Count nodes                                  *\n");
    printf("* 5 --> Display list data                            *\n");
    printf("* 6 --> Delete node                                  *\n");
    printf("* 7 --> Save integer list to file                    *\n");
    printf("* 8 --> Load integer list from file                  *\n");
    printf("* 9 --> Quit program                                 *\n");
    printf("*                                                    *\n");
    printf("******************************************************\n\n");
    
    do //guard loop
    {
        printf("Enter your choice >");
        scanf("%d", &selection);
    } while (selection<1 || selection>9);
    printf("\n");
    
    return(selection);
}//end_menu_selection

//==============================================================================

void menuReturn()
{
   printf("\npress any key to return to main menu");
   getch();
   system("cls");
}//end_menuReturn

//==============================================================================

void MemChk(struct node* newp)
{
   if (!newp)
   {
      printf("\nout of memory\n");
      printf("press any key to exit");
      getch();
      exit(1);
   }
   else
   {
      printf("\nmemory allocated successfully\n");
   }
}         

//==============================================================================

void AddNode(struct node** posRef, int data)            //adds a node after the desired
{                                                       //position "posRef"
   struct node* newNode = malloc(sizeof(struct node));  //posRef == head, tail etc.

   MemChk(newNode);
   newNode->info = data;
   newNode->next = *posRef;
   *posRef = newNode;
}//end_AddNode   

//==============================================================================

void getData(int* dataRef)
{
   int temp;
   
   printf("give an integer number\n>");
   scanf("%d", &temp);
   *dataRef = temp;
}//end_getData   
   
//==============================================================================

void LocateTail(struct node* head, struct node** posRef)
{
   struct node* current = head;
   
   while (current->next != NULL)
   {
      current = current->next;
   }   
                       //*posRef (position) contains the address
   *posRef = current;  //of the final node 
}//end_LocateTail                     
                   
//==============================================================================

int CountNodes(struct node* head)
{
   struct node* current = head;
   int counter = 0;
   
   while (current != NULL)
   {
      counter++;
      current = current->next;
   }
   
   return(counter);
}//end_CountNodes  

//==============================================================================

int SearchNode(struct node* head, int* nNum, int data)
{
   struct node* current = head;
   int count = 0, exists = 0;
   
   while ((current->info != data) && (current !=NULL))
   {
      count++;
      current = current->next;
   }

   if (current != NULL)
      exists = 1;
   *nNum = count;
   return(exists);
}//end_SearchNode

//==============================================================================

void displayList(struct node* head)
{
   struct node* current = head;
   int line = 0;
   
   while (current != NULL)
   {
      line++;
      printf("%d\n", current->info);
      if (line == 10)
      {
         printf("\npress any key to see the next 10 numbers\n\n");
         getch();
         line = 0;
      }
      current = current->next;
   }
}//end_displayList
      
//==============================================================================

int main()
{
   int data, quit = 0;
   int nodeNum = 0;
   struct node* head = NULL;
   struct node* position = NULL;
   
   while (quit == 0)
   {
      switch( menu_selection() )
      {
         case 1:{                      //add at tail
                   getData(&data);
                   if (head == NULL)      
                   {                         
                      AddNode(&head, data);  
                   }
                   else
                   {   
                      LocateTail(head, &position);
                      AddNode(&position->next, data);
                   }   
                   break;
                }
//add at head         
         case 2:{                      
                   getData(&data);
                   AddNode(&head, data);
                   break;
                }
//Search node
         case 3:{                    
                   getData(&data);
                   if (head == NULL)
                   {
                      printf("The list is empty\n");
                   }
                   else if (SearchNode(head, &nodeNum, data) == 1)
                   {
                      printf("Node with data number %d found at position %d\n", data, nodeNum);
                   }
                   else
                   {
                      printf("Node with data number %d does not exist\n");
                   }
                   break;
                }
//count nodes
         case 4:{
                   printf("There are %d nodes in the list\n", CountNodes(head) );
                   break;
                }   
//display list data
         case 5:{
                   displayList(head);
                   printf("\nend of list!\n");
                   break;
                }   

         case 9:{
                   quit = 1;
                   printf("press any key to quit");
                   getch();
                   break;
                 }
                 
         default: exit(1);
      }//end_switch
      if (quit != 1)
            menuReturn();
   }//end_while

   return(0);
}//end_main
