/*
File: my_allocator.c

Author: <your name>
Department of Computer Science
Texas A&M University
Date  : <date>

Modified:

This file contains the implementation of the module "MY_ALLOCATOR".

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "my_allocator.h"
#include <math.h>
#include <stdbool.h>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

typedef struct node {
	struct node* next;
	int size;
	bool filled;
} node;

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

Addr head;
char* begin;
char* end;
int baseSize;
int memLength;
int listSize;
node** listHead;

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR MODULE MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/
/* Don't forget to implement "init_allocator" and "release_allocator"! */
int roundToPow2(int length)
{
	int next = (int)pow(2, ceil(log2(length)));
	return next;
}

int makeKey(int length)
{
	int key = 0;
	for (int i = 0; i < listSize; i++)
	{
		if ((memLength / pow(2, i)) == length)
		{
			key = i;
			return key;
		}
	}
	return -1;
}

Addr getBuddy(Addr a, int size)
{
	Addr start = (Addr)((unsigned long long)a - (unsigned long long)head);
	Addr buddy = (Addr)((unsigned long long)start ^ size);
	return (Addr)((unsigned long long)buddy + (unsigned long long)head);
}

Addr split(int wSize, int inc)
{
	Addr ret = listHead[inc];
	node* temp;
	char* buddy;
	int newSize = (memLength / pow(2, inc)) / 2;
	if (listHead[inc] == NULL)
	{
		return split(wSize, inc - 1);
	}
	else if (wSize == newSize)
	{
		temp = listHead[inc];
		listHead[inc] = temp->next;
		listHead[inc + 1] = temp;
		buddy = (char*)listHead[inc + 1];
		ret = (Addr)buddy;
		ret = getBuddy(ret, newSize);
		listHead[inc + 1]->next = (node*)ret;
		listHead[inc + 1]->size = newSize;
		listHead[inc + 1]->filled = false;
		temp = listHead[inc + 1]->next;
		temp->next = NULL;
		temp->filled = false;
		temp->size = newSize;
		return (Addr)listHead[inc + 1];
	}
	else
	{
		temp = listHead[inc];
		listHead[inc] = temp->next;
		listHead[inc + 1] = temp;
		buddy = (char*)listHead[inc + 1];
		ret = (Addr)buddy;
		ret = getBuddy(ret, newSize);
		listHead[inc + 1]->next = (node*)ret;
		listHead[inc + 1]->size = newSize;
		listHead[inc + 1]->filled = false;
		temp = listHead[inc + 1]->next;
		temp->next = NULL;
		temp->filled = false;
		temp->size = newSize;
		return split(wSize, inc + 1);
	}
	return NULL;
}



unsigned int init_allocator(unsigned int _basic_block_size, unsigned int _length)
{
	if (head == NULL)
	{
		baseSize = roundToPow2(_basic_block_size);
		memLength = roundToPow2(_length);
		listSize = log2(ceil(memLength / baseSize)) + 1;
		listHead = (node**)malloc(listSize * sizeof(node));
		head = malloc(memLength);
		begin = (char*)head;
		end = begin + memLength;
		node* first;
		first = (node*)head;
		first->next = NULL;
		first->size = memLength;
		first->filled = false;
		memcpy(head, first, sizeof(node));
		listHead[0] = (node*)head;
		for (int i = 1; i <= listSize; i++)
		{
			listHead[i] = NULL;
		}
		return memLength;
	}
	else
	{
		printf("Allocator already initialized!\n");
		return 0;
	}
}
/* This function initializes the memory allocator and makes a portion of
’_length’ bytes available. The allocator uses a ’_basic_block_size’ as
its minimal unit of allocation. The function returns the amount of
memory made available to the allocator. If an error occurred,
it returns 0.
*/

int release_allocator()
{
	free(listHead);
	free(head);
}
/* This function returns any allocated memory to the operating system.
After this function is called, any allocation fails.
*/


extern Addr my_malloc(unsigned int _length) {
	//find the smallest possible memory size
	/* This preliminary implementation simply hands the call over the
	the C standard library!
	Of course this needs to be replaced by your implementation.
	*/
	//insert into listHead 
	//find address where memory would fit
	//return that address
	Addr loc = head;
	char* move;
	node* temp;
	_length += sizeof(node);
	if (_length < baseSize)
	{
		_length = baseSize;
	}
	_length = roundToPow2(_length);
	int key = makeKey(_length);
	if (_length < 0)
	{
		printf("Negative memory length is not allowed.\n");
		return NULL;
	}
	else if (_length > memLength)
	{
		printf("Requested length exceeds total allowed memory.\n");
	}
	else
	{
		if (listHead[key] == NULL)
		{
			loc = split(_length, key);
			temp = (node*)loc;
			listHead[key] = listHead[key]->next;
			temp->next = NULL;
			temp->filled = true;
			move = (char*)loc;
			move = move + sizeof(node);
			loc = (Addr)move;
			return loc;
		}
		else
		{
			listHead[key]->filled = true;
			listHead[key]->next = NULL;
			temp = listHead[key];
			listHead[key] = NULL;
			move = (char*)temp;
			move = move + sizeof(node);
			return (Addr)move;
		}

	}
	return NULL;
}

Addr merge(int cSize, node* a)
{
	node* temp = a;
	Addr buddy;
	int key = makeKey(temp->size);
	if (key > listSize - 1)
	{
		return NULL;
	}
	if (cSize == memLength)
	{
		listHead[0] = a;
		for (int i = 1; i <= listSize; i++)
		{
			listHead[i] = NULL;
		}
		return (Addr)a;
	}
	buddy = (Addr)getBuddy(temp, temp->size);
	node* bud = (node*)buddy;
	if (bud->filled == false && bud->size == temp->size)
	{
		Addr ret;
		node* next = listHead[key - 1];
		node * check;
		node* store;
		if ((unsigned long long)temp < (unsigned long long)bud)
		{
			check = listHead[key - 1];
			if (check == NULL)
			{
				listHead[key - 1] = temp;
			}
			else if (check->next == NULL)
			{
				listHead[key - 1] = temp;
			}
			else
			{
				store = check;
				listHead[key - 1] = temp;
				listHead[key - 1]->next = store;
			}
			listHead[key - 1] = temp;
			temp->size = temp->size * 2;
			node* current = listHead[key];
			if (current == NULL)
			{
				listHead[key - 1] = temp;
			}
			else if (current->next != NULL)
			{
				listHead[key] = current->next;
			}
			else
			{
				listHead[key] = NULL;
			}
			return merge(temp->size, listHead[key - 1]);
		}
		else
		{
			check = listHead[key - 1];
			if (check == NULL)
			{
				listHead[key - 1] = bud;
			}
			else if (check->next == NULL)
			{
				listHead[key - 1] = bud;
			}
			else
			{
				store = check;
				listHead[key - 1] = temp;
				listHead[key - 1]->next = store;
			}
			bud->size = bud->size * 2;
			node* current = listHead[key];
			if (current == NULL)
			{
			}
			else if (current->next != NULL)
			{
				listHead[key] = current->next;
			}
			else
			{
				listHead[key] = NULL;
			}
			return merge(bud->size, listHead[key - 1]);
		}
	}
}

extern int my_free(Addr _a) {
	/* Same here! */
	printf("BEFORE: \n");
	printFreeNodes();
	node* temp = _a;
	if ((unsigned long long)_a > (unsigned long long)end)
	{
		return 1;
	}
	Addr check;
	char* address = (char*)_a;
	int roundSize = 0;
	address = address - sizeof(node);
	temp = (node*)address;
	if (temp == NULL)
		return -1;
	else
	{
		int freeSize = temp->size;
		printf("Trying to free size %d and merge if possible\n", freeSize);
		temp->filled = false;
		check = merge(temp->size, temp);
		printf("AFTER: \n");
		printFreeNodes();
		if (check == NULL)
			return -1;
		else return 1;
	}

}

void printFreeNodes() {
	printf("Free Nodes: \n");
	for (int i = 0; i < listSize; i++) {
		int count = 0;
		node* a = listHead[i];
		int totalSize = memLength / pow(2, i);
		printf("in key %d of possible size %d", i, totalSize);
		while (a != NULL) {
			count++;
			printf("size: %d", a->size);
			a = a->next;
		}
		printf(" total nodes : %d \n", count);
	}
}