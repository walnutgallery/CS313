#include "my_allocator.h"
#include <unistd.h>

void onExit() {
	release_allocator();
}

int main(int argc, char ** argv) {
	atexit(onExit);
	int b = 128;
	int M = 134217728;  
	int cLine;
	opterr = 0;

	while ((cLine = getopt(argc, argv, "b:s:")) != -1)
		switch (cLine)
		{
		case 'b':
			b = atoi(optarg);
			break;
		case 's':
			M = atoi(optarg);
			break;
		case '?':
			if (optopt == 'b', optopt == 's')
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf(stderr,
					"Unknown option character `\\x%x'.\n",
					optopt);
			return 1;
		}

	init_allocator(b, M);
	ackerman_main();
	
	
}