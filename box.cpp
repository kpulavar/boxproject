/*
 * box.cpp
 *
 *  Created on: Mar 25, 2017
 *      Author: K Pulavarti
 *
 *  Notes: The ISO Base file format data is ideally arranged for an OO model.However, since
 *  this simplified exercise does not require any saving of the tree structure of nodes, or
 *  any box data, except for mdat, a simple C-style recursion will suffice. The data is already
 *  arranged in a tree structure, so a simple recursion is implemented here.
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>




// Process mdat file contents
int processMDAT(FILE *fp, unsigned int size)
{
	char *mdatBuffer = new char[size];


	if (fread(mdatBuffer,1,size,fp) !=size)
	{
		printf("Could not read MDAT data \n");
		return 0;
	}
	printf("%s\n", mdatBuffer);
	delete [] mdatBuffer;
	return 1;
}

// Recursive function to process boxes and their children
int processBox(FILE *fp)
{
	unsigned int boxSize;
	unsigned char boxType[5] = {0};

	if(fread(&boxSize,4,1,fp) != 1)
	{
		return 0;
	}
	boxSize = ntohl(boxSize);
	if(fread(boxType,4,1,fp) != 1)
	{
		return 0;
	}

	printf("Found box of type %s and size %d\n", boxType, boxSize);

	// MOOF and TRAF boxes have other boxes in them.
	if(!strcmp("moof",(char*)boxType) || !strcmp("traf",(char *)boxType))
		{
			// These boxes have boxes embedded
			return processBox(fp);
	    }
	// Process an mdat box wherever it occurs
	if(!strcmp("mdat", (char *)boxType))
	{
		return processMDAT(fp, boxSize -8);
	}

	else
	{
		// All other boxes, discard the data
		if(fseek(fp, boxSize -8,SEEK_CUR))
		{
			return 0;
		}
	}
	return 1;
}

int main(int argc, char **argv)
{

	FILE *fp;

	if(argc < 2){
		printf("Usage: box <filename>\n");
		exit(0);
	}

	fp = fopen(argv[1],"rb");

	if(!fp){
			printf("File not found\n");
			exit(0);
	}

	while(processBox(fp));
}



