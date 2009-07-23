#include <stdio.h>
#include "invertedList.h"
main(){
	invertedList inverList;

	inverList.insertItem( 0, 0);
	inverList.insertItem( 3, 0);
	inverList.insertItem( 8, 0);
	inverList.insertItem( 9, 0);



	inverList.insertItem( 0, 1);
	inverList.insertItem( 3, 1);
	inverList.insertItem( 8, 1);
	inverList.insertItem( 9, 1);


	inverList.insertItem( 0, 2);
	inverList.insertItem( 3, 2);
	inverList.insertItem( 8, 2);
	inverList.insertItem( 9, 2);

//	int lau[] = {0, 3, 8, 9};
	int lau[] = {0, 3, 8, 9};

	printf("mergeResult = %d\n", inverList.mergeLists2(4, lau));

}
