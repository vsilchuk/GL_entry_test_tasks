#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int spans = 0, dist = 0, length = 0;

	if (argc != 3) {
		fprintf(stderr, "How to use:\t%s <spans_number_N> <distance_D>\n", argv[0]);
		return -1;	// exit(EXIT_FAILURE);
	} else {
		spans = atoi(argv[1]);
		dist = atoi(argv[2]);

		if (spans < 0 || dist < 0) {
			fprintf(stderr, "spans_number_N and distance_D should be >= 0\n");
			return -1;	// exit(EXIT_FAILURE);
		}
	}

	length = (dist * spans * (1 + spans)) / 2;
	printf("%d\n", length);

	return 0;
}

