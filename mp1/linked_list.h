typedef struct Node {
	struct Node* next;
	int key;
	int size;
	char val;

}Node;

	int Insert(int x, char* value_ptr, int value_len);
	/*
	inserts a key-value pair, where the key is an integer, x,
	and the value is some data pointed to by the pointer value \_ptr
	of length value \_len. You should use the library function, memcpy() to copy the value.
	*/

	void Delete(int x);
	/*
	Deletes the first item from the beginning with the key x.
	*/

	char* Lookup(int x);
	/*
	Finds the given key in a list and returns a pointer to the first occurrence of that key.
	*/

	void PrintList();
	/*
	Prints out all the items' key-value pairs sequentially starting from the head of the list.
	Print only the key and the value-length. Do not print the actual value as it could countain
	binary/non-printable data.
	*/

	void Init(int M, int C);
	/*
	Initially obtains M bytes of memory by calling the std lib function malloc() and initializes the linked list
	*/

	void Destroy();
	/*
	Destroy the linked list and return memory to the system by calling another lybrary function, free()
	*/
