#include <stack.h>
#include <stdio.h>
#include <stdlib.h>

int main(int arg, char **argv){
	Stack *st = stCreate();

	int nums[] = {0, 1, 2, 3, 4};

	int i;
	for (i=0;i<5;i++){
		stPush(st, &nums[i]);
	}

	int *num;
	//take 4 out
	stPop(st, (void**)&num);
	printf("num: %d\n", num[0]);

	//print top
	stGetTop(st, (void**)&num);
	printf("Top: %d\n", num[0]);
	
	//add 5 three times
	int five = 5;

	stPush(st, &five);
	stPush(st, &five);
	stPush(st, &five);

	stGetTop(st, (void**)&num);
	printf("Top: %d\n", num[0]);

	printf("num elements: %d\n", stNumElements(st));
	
	//print elements
	while (stPop(st, (void**)&num) != -1){
		printf("num: %d\n", num[0]);
	}

	printf("num elements: %d\n", stNumElements(st));

	if (stGetTop(st, (void**)&num) == -1){
		printf("Cant get top!\n");
	}

	stDestroy(&st, ST_DM_KEEP);
	return 0;
	
}
