#include "linked_list.h"

int llist_insert(CharLinkedList *list, char newElement, int position) {

  // alocate memory for a new node with the given char
  CharLinkedListNode *newNode = malloc(sizeof(CharLinkedListNode));
  newNode->element = newElement;
  newNode->next = NULL;
  CharLinkedListNode *currentNode = list->header->next;

  int i = 0;

  if( position == 0){
    newNode->next = list->header->next;
    list->header->next = newNode;
    list->size++;
    return 0;
  }

  while (currentNode != NULL) {

    if (i == position - 1) {

      CharLinkedListNode *nextNode = currentNode->next;
      currentNode->next = newNode;
      newNode->next = nextNode;
      list->size++;
      return 0;
    }

    currentNode = currentNode->next;
    i++;
  }

  CharLinkedListNode *nextNode = currentNode->next;
  currentNode->next = newNode;
  newNode->next = nextNode;
  list->size++;

  return 0;
}

int llist_delete(CharLinkedList *list, int position) {

  CharLinkedListNode *currentNode = list->header->next;

  int i = 0;

  if( position == 0){

    CharLinkedListNode* deletedNode = currentNode;
    list->header->next = deletedNode->next;
    free(deletedNode);
    list->size--;
    return 0;
  }

  while (currentNode != NULL) {

    if (i == position-1) {
      
      CharLinkedListNode* deletedNode = currentNode->next;
      currentNode->next = deletedNode->next;
      free(deletedNode);
      list->size--;
      return 0;
    }

    currentNode = currentNode->next;
    i++;
  }

  return -1;

}

char llist_pop(CharLinkedList *list){

  char result = llist_retreive(list,0);
  llist_delete(list,0);

  return result;
}

char llist_retreive(CharLinkedList *list, int position){

  CharLinkedListNode *currentNode = list->header->next;

  int i = 0;

  while (currentNode != NULL) {

    if (i == position) {

      return currentNode->element;
    }

    currentNode = currentNode->next;
    i++;
  }

  return '\0';
}

void llist_print(CharLinkedList* list){

  printf("Printing list \n");
  CharLinkedListNode *currentNode = list->header->next;
  int i = 0;

  while (currentNode != NULL) {

    printf("%d - %c \n",i,currentNode->element);
    currentNode = currentNode->next;
    i++;
  }

}

int llist_empty(CharLinkedList *list){

  while(list->size != 0){

    llist_delete(list,0);
  }
  return 0;
}

bool llist_is_empty(CharLinkedList* list){

  if(list->header->next == NULL)
    return true;
  else
    return false;

}

