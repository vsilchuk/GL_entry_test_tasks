#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int stairway_len = 0, step_over = 0, current_step, steps = 0;

	if (argc != 3) {
		fprintf(stderr, "How to use:\t%s <stairway_len_N> <step_over_K>\n", argv[0]);
		return -1;
	} else {
		stairway_len = atoi(argv[1]);
		step_over = atoi(argv[2]);

		if (stairway_len < 0 || step_over < 0) {
			fprintf(stderr, "stairway_len_N and step_over_K should be >= 0\n");
			return -1;	
		}
	}

	for (current_step = 0; current_step < stairway_len; current_step += (step_over + 1)) {
		steps++;
		// printf("%d ", current_step);	// use it to show the steps indices
	}

	// printf("\n");	// in addition to the previous commented code		

	printf("%d\n", steps);
	return 0;
}

