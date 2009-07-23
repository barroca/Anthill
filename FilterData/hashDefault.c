#include <stdio.h>
#include <string.h>

/**
 * This is the default hash, used by labeled stream if user does not provide one. Hash is used by the labeled stream
 * after the getLabel extracts the label from the message. After hashing the label, we have the destination of the
 * message.
 * \param label the label of the message, extracted by getLabel
 * \param image the number of destinations valid
 * \return the destination instance, something >=0 and < image
 */ 
int hashDefault(char *label, int numDestinations){
	int i, labelSize, key = 0;
	labelSize = strlen((char *)label);
	
	for(i = 0; i < labelSize; i++){
		key +=  (int)label[i];
	}
	return(key % numDestinations);
}


/**
 * Similar to default hash, but the result is a set of destinations, not a single one. The array size is given by the
 * the image parameter.
 * \param label the label of the message, extracted by getLabel
 * \param image the number of destinations valid
 * \param destArray a "boolean" array, where each positions says if a destination is valid or not. \
	The array size is given by the image parameter. Void allocates it.
 */
void mlsHashDefault(char *label, int numDestinations, int destArray[]){
	int i;

	for (i=0;i<numDestinations;i++){
		
		int j, labelSize, key = 0;
		labelSize = strlen((char *)label);

		for(j = 0; j < labelSize; j++){
			key +=  (int)label[j];
		}
		key = key % 2; //true or false

		destArray[i] = key;
	}

}
