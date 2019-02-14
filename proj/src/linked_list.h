#include <lcom/lcf.h>

/** @defgroup CharLinkedList CharLinkedList
 * @{
 * Functions implement a character linked list
 */

/** @name CharLikedListNode */
/**@{
 *
 * @brief Basic component of a char linked list
 * 
 */
typedef struct CharLinkedListNode{

  char element;                      /**< @brief char stored */
  struct CharLinkedListNode* next;   /**< @brief pointer to next node (NULL if last node) */

}CharLinkedListNode;

/** @name CharLikedList */
/**@{
 *
 * @brief Simple linked list containing chars
 * 
 */
typedef struct CharLinkedList{

    CharLinkedListNode* header;     /**< @brief pointer to the header node(this node is not valid) */
    size_t size;                    /**< @brief number of valid elements of a list */ 
    
}CharLinkedList;

/**
 * @brief Inserts a given char in the given char linked list
 * 
 * @param list            pointer to a char linked list
 * @param newElement      new element to be inserted
 * @param position        position where the new element should be inserted
 * @return int            0 if sucessful, non-zero otherwise
 */
int llist_insert(CharLinkedList* list,char newElement, int position);

/**
 * @brief Deletes a given element of a char linked list
 * 
 * @param list            pointer to a char linked list
 * @param position        position of the element to be deleted
 * @return int            0 if sucessful, non-zero otherwise
 */
int llist_delete(CharLinkedList* list, int position);

/**
 * @brief Removes the element of the head of the list and returns it
 * 
 * @param list            pointer to a char linked list
 * @return char           element that was at the top of the list
 */
char llist_pop(CharLinkedList* list);

/**
 * @brief Retreives the element that is a the given position on the list
 * 
 * @param list            pointer to a char linked list
 * @param position        position of the element to be retreived
 * @return char           
 */
char llist_retreive(CharLinkedList* list, int position);

/**
 * @brief Prints the given char linked list in the form: ordinal number-char
 * 
 * @param list            pointer to a char linked list
 */
void llist_print(CharLinkedList* list);

/**
 * @brief Empties the given char linked list
 * 
 * @param list            pointer to a char linked list
 * @return int            zero if sucessful, non-zero otherwise
 */
int llist_empty(CharLinkedList *list);

/**
 * @brief Checks if a given linked list is empty
 * 
 * @param list             pointer to a char linked list
 * @return true            the list is empty
 * @return false           the list is not empty
 */
bool llist_is_empty(CharLinkedList* list);

/**@} */
