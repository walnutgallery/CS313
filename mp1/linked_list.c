#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linked_list.h"

Node* head;
Node* tail;
Node* fp;
Node* iterator;
int nodeNum;
int nodeSize;
int ptrSize = 8;

int Insert(int x, char* value_ptr, int value_len)
{
	char* fpMove;
	if ((nodeSize - 2*sizeof(int) - ptrSize)< value_len) //check if required operation exceeds alotted bytes per node for value
	{
		printf("Insert failed, item exceeds size.\n");
		return -1;
	}
	if (nodeNum == 0) //check if the list is full
	{
		printf("List full, insertion failed.\n");
		return -1;
	}
	if (head == NULL) //check if first insertion
	{
		head = fp;
		tail = head;
	}
	Node* copy; //node pointer
	copy = fp;
	fpMove = (char*)fp;
	fpMove = fpMove + nodeSize;
	copy->next = NULL;
	copy->key = x;
	copy->size = value_len;
	memcpy(&copy->val, value_ptr, value_len); //copies all of the values
	tail->next = fp; //set next to the new node
	tail = fp; //move the tail to the new node
	fp = (Node*)fpMove; //move the free pointer by nodeSize
	nodeNum = nodeNum - 1;
	return 1;
}

void Delete(int x)
{
	Node* iter = head;
	Node* temp = head;
	iterator = head;
	int find = 0;
	char* fpMove;
	while (iter != NULL)
	{
		find = iter->key;
		if (find == x)
		{
			printf("Found Node with key %d. Deleting...\n", x);
			iterator = iter;
			fpMove = (char*)iterator;
			while (iterator->next != NULL)
			{
				temp = iterator->next;//get next node
				memcpy(fpMove, temp, nodeSize);//copy data of next node into previous node
				fpMove = fpMove + nodeSize;//move iteration free pointer
				iterator->next = (Node*)fpMove;//set current node to point to new, address, that of the directly next node
				iterator = (Node*)fpMove;//proceed with iteration
			}
			nodeNum = nodeNum + 1;
			tail = (Node*)(fpMove - nodeSize);
			fp = (Node*)fpMove;
			tail->next = NULL;
			return;
		}
		iter = iter->next;
	}
	printf("No Node with key %d found to Delete.\n", x);
}

char* Lookup(int x)
{
	char* ret = NULL;
	char* iter = (char*)head;
	Node* temp = head;
	int find = 0;
	while (iter != NULL)
	{
		temp = (Node*)iter;
		find = temp->key; //get the key
		if (find == x) 
		{
			ret = (char*)&temp->key; //return the address of the key
			return ret;
		}
		iter = (char*)temp->next;
	}
	printf("No such key, Lookup failed.\n"); //in case of no key matching x
	return ret;
}

void PrintList()
{
	Node* iter = head;
	int key = 0;
	int len = 0;
	while(iter != tail->next) //iterate through the list, outputting key and value length
	{
		if(iter == tail->next)
			break;
		key = iter->key;
		len = iter->size;
		printf("%d(L%d), ", key, len);
		iter = iter->next;
	}
	printf("\n");
}

void Destroy()
{
	free(head);
}

void Init(int M, int C) //intializes list and key global variables
{
	head = NULL;
	tail = NULL;
	fp = malloc(M);
	iterator = head;
	nodeNum = M / C;
	nodeSize = C;
}
