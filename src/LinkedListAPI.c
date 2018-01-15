#include "LinkedListAPI.h"
#include <assert.h>

List initializeList(char * (*printFunction)(void *toBePrinted),void (*deleteFunction)(void *toBeDeleted),int (*compareFunction)(const void *first,const void *second))
{
	//New List pointer
	List list;
		
	//malloc the head pointer
	list.head = NULL;
	list.tail = NULL;
	
	//Set the function pointers to the functions passed in
	assert(printFunction != NULL);
	assert(deleteFunction != NULL);
	assert(compareFunction != NULL);
	list.printData = printFunction;
	list.deleteData = deleteFunction;
	list.compare = compareFunction;
	return list;
}

Node *initializeNode(void *data)
{
	if (data != NULL)
	{
		Node* node = malloc(sizeof(Node));
		node->data = NULL;
		node->next = NULL;
		node->previous = NULL;

		node->data = data;
		return node;
	}
	return NULL;
}

void insertFront(List *list, void *toBeAdded)
{
	if (list == NULL || toBeAdded == NULL)
	{
		return;
	}
	else if (list->head == NULL && list->tail == NULL)
	{// if no elements create element
		Node* newNode = NULL;
		newNode = initializeNode(toBeAdded);
		list->head = newNode;
		list->tail = newNode;
		return;
	}
	else if (list->head != NULL)
	{
		Node* newNode = NULL;
		newNode = initializeNode(toBeAdded);
        list->head->previous = newNode;
        newNode->next = list->head;
        list->head = newNode;		
		return;
	}
}

void insertBack(List *list, void *toBeAdded)
{
	if (list == NULL || toBeAdded == NULL)
	{
		return;
	}
	else if (list->tail == NULL && list->head == NULL)
	{// if no elements create element
		Node* newNode = NULL;
		newNode = initializeNode(toBeAdded);
		list->head = newNode;
		list->tail = newNode;
		return;
	}
	else if (list->tail != NULL)
	{
		Node* newNode = NULL;
		newNode = initializeNode(toBeAdded);
        list->tail->next = newNode;
        newNode->previous = list->tail;
        list->tail = newNode;		
		return;
	}
}

void printForward(List * list)
{
	if (list == NULL)
	{
		return;
	}
	else if ((list != NULL) && (list->head != NULL))
	{
		Node* tempPtr = list->head;
		while(tempPtr!= NULL)
		{
			char * print;
			print = list->printData(tempPtr->data);
			printf("%s\n", print);
			tempPtr = tempPtr->next;
		}
	}
}

void printBackwards(List * list)
{
	if (list == NULL)
	{
		return;
	}
	else if ((list != NULL) && (list->tail != NULL))
	{
		Node* tempPtr = list->tail;
		while(tempPtr != NULL)
		{
			char * print;
			print = list->printData(tempPtr->data);
			printf("%s\n", print);
			tempPtr = tempPtr->previous;
		}
	}
}

void * getFromBack(List list)
{
	if(list.tail == NULL)
		return NULL;
	return list.tail->data;
}

void * getFromFront(List list)
{	
	if(list.tail == NULL)
		return NULL;
	return list.head->data;
}

void clearList(List *list)
{
	/* DNE */
	if (list == NULL || list->tail == NULL || list->head == NULL) // Null List || Empty List
	{
		return;
	}	
	if ((list != NULL)&&(list->head != NULL)&&(list->tail != NULL))
	{
		Node* currentNode = list->head;
		Node* nextNode;
		
		while(currentNode != NULL)
		{
			nextNode = currentNode->next;
			list->deleteData(currentNode->data);
			free(currentNode);
			currentNode = NULL;
			currentNode = nextNode;
		}
		list->head = NULL;
		list->tail = NULL;
		return;	
	}
	return;
}

void * deleteDataFromList(List *list, void *toBeDeleted)
{
	if (list == NULL || toBeDeleted == NULL)
	{	
		return NULL;
	}
	if (list->head == NULL || list->tail == NULL)
	{
		return NULL;
	}
	if ((list != NULL)&&(list->head != NULL))
	{	
		Node* tmpPtr1 = NULL;
		Node* tmpPtr2 = NULL;		
		Node* tmpPtr = NULL;
		tmpPtr = list->head;
		
		// 1 element in List
		if(tmpPtr->previous == NULL && tmpPtr->next == NULL)
		{
			if( list->compare(tmpPtr->data, toBeDeleted) == 0 )
			{
				void * data = tmpPtr->data;
				list->tail = NULL;
				list->head = NULL;
				free(tmpPtr);
				return data;	
			}
		}
		else 
		{
			while( tmpPtr != NULL )
			{
				if( list->compare(tmpPtr->data, toBeDeleted) == 0 )
				{	
					/* Element is the first element */
					if ((tmpPtr->previous == NULL)&&(tmpPtr->next != NULL))
					{
						void * data = tmpPtr->data;
						tmpPtr1 = tmpPtr->next;
						tmpPtr1->previous = NULL;
						list->head = tmpPtr1;
						free(tmpPtr);
						return data;
					}
					/* Element is the last element */
					else if ((tmpPtr->next == NULL)&&(tmpPtr->previous != NULL))
					{
						void * data = tmpPtr->data;
						tmpPtr1 = tmpPtr->previous;
						tmpPtr1->next = NULL;
						list->tail = tmpPtr1;
						free(tmpPtr);
						return data;
					}
					/* Element is in the middle */
					else if ((tmpPtr->next != NULL)&&(tmpPtr->previous != NULL))
					{
						void * data = tmpPtr->data;
						tmpPtr1 = tmpPtr->previous;
						tmpPtr2 = tmpPtr->next;
						
						tmpPtr1->next = tmpPtr2;
						tmpPtr2->previous = tmpPtr1;
						free(tmpPtr);
						return data;
					}
				}
				tmpPtr = tmpPtr->next;
			}
		}
	}
	return NULL;
}

void insertSorted(List *list, void *toBeAdded)
{
	/* List DNE */
	if (list == NULL)
		return;
		
	/* List Exists, but no items, just add to the front */
	if ((list != NULL) && (list->head == NULL))
	{
		insertFront(list, toBeAdded);
		return;
	}
	/* otherwise insertSorted node in list*/
    else {
        Node * currNode = list->head;
        Node * nodeToBeAdded = initializeNode(toBeAdded); /* create new node */

        while(currNode->next != NULL) {
            if (list->compare(currNode->data, nodeToBeAdded->data) < 0) 
            {
                currNode = currNode->next;
            }
            else if (list->compare(currNode->data, nodeToBeAdded->data) >= 0) 
            {
                //insert new node in current node position
                nodeToBeAdded->next = currNode;
                nodeToBeAdded->previous = currNode->previous;
                if (currNode->previous != NULL) 
                {
                    currNode->previous->next = nodeToBeAdded;
                }
                else 
                {
                    list->head = nodeToBeAdded;
                }
                currNode->previous = nodeToBeAdded;
                return;
            }
        }
        //this section checks last value in list/only value
        if (currNode->next == NULL) 
        {
            if (list->compare(currNode->data, nodeToBeAdded->data) < 0) 
            {
                //insert newNode after current node
                currNode->next = nodeToBeAdded;
                nodeToBeAdded->previous = currNode;
                //set list tail as newNode since it's now at the end
                list->tail = nodeToBeAdded;
            }
            else if (list->compare(currNode->data, nodeToBeAdded->data) >= 0) 
            {
                if (currNode->previous != NULL) 
                {
                    nodeToBeAdded->next = currNode;
                    nodeToBeAdded->previous = currNode->previous;
                    currNode->previous->next = nodeToBeAdded;
                    currNode->previous = nodeToBeAdded;
                }
                else 
                {
                    currNode->previous = nodeToBeAdded;
                    nodeToBeAdded->next = currNode;
                    //set list head as new node since it's now at the beginning
                    list->head = nodeToBeAdded;
                }
            }
        }
    }
	return;	
}

char * toString(List list)
{
	int length = 0;
	Node * tmp = list.head;
	char * string;
	while( tmp != NULL )
	{
		string = list.printData(tmp->data);
		length = length + strlen(string) + 1;
		free(string);
		tmp = tmp->next;
	} 
	char * listString;
	listString = malloc(sizeof(char)*length);
	Node * tmp2 = list.head;
	int flag = 0;
	char * tPtr;
	while( tmp2 != NULL )
	{
		tPtr = list.printData(tmp2->data);
		length = length + strlen(tPtr) + 1;
		listString = realloc(listString, length);
		if(flag == 0){
			strcpy(listString, tPtr);
		}
		else{
			strcat(listString, tPtr);
		}
		tmp2 = tmp2->next;
		free(tPtr);
		flag = 1;
	}
	return listString;
}

ListIterator createIterator(List list)
{
	ListIterator iter;
	iter.current = list.head;
	return iter;
}

void * nextElement(ListIterator * iter)
{
	if(iter->current != NULL)
	{	
		void * save = iter->current->data;
		iter->current = iter->current->next;
		return save;
	}else
		return NULL;
}

void* findElement(List list, bool (*customCompare)(const void* first,const void* second), const void* searchRecord){
	if( (&list)->head == NULL || customCompare == NULL || searchRecord == NULL ){
		return NULL;
	}
	Node * tPtr;
	tPtr = list.head;
	while( tPtr != NULL ){
		if(customCompare(tPtr->data, searchRecord)){
			return tPtr->data;
		}
		tPtr = tPtr->next;
	}
	return NULL;
}

int getLength(List list){
	if((list.head != NULL && list.tail == NULL) || (list.head == NULL && list.tail != NULL)){
		return -1;
	}
	int length = 0;
	Node * tPtr = NULL;
	tPtr = list.head;
	while( tPtr != NULL ){
		length++;
		if(tPtr->next == NULL){
			break;
		}
		tPtr = tPtr->next;
	}
	return length;
}