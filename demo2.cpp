/*
 Test program to show how the disk emulator API works.
 Author: D.M. Lyons
 Date: Spring 2017

 c 2017 dml Fordham CIS
*/

#include<iostream>
#include<fstream>
#include<vector>
#include <map>
#include <stdlib.h>

using namespace std;

#include "diskprocess.h" // the API/definitions for disk emulator
#include "linkedlist.cpp" //linked list for free space management 


int myBlockSize= 5;
int myDiskSize = 10;
linked_list freeSpace(myBlockSize);
DiskProcessType myDisk(myBlockSize,myDiskSize);
std::map<std::string, int> directory;


//Strings: 
string MEMORYFULLMSG = "\nDirectory is full. Please delete a file and try again...\n";
string FILENOTFOUND = "\nThe file you entered was not found. Please try again...\n";


void createFile (string file){
	if (!freeSpace.hasFreeSpace())
		cout << MEMORYFULLMSG;
	else{
		directory [file]= freeSpace.getNextBlock();
//		DiskBlockType *myBuffer = new DiskBlockType(myBlockSize);
//		myDisk.write(directory[file],myBuffer);
	}
}


//helper function that returns next block to be added to the index block
int addBlock(string file){
	int blockNo = directory[file];
	
	DiskBlockType *myBuffer = new DiskBlockType(myBlockSize);
	myDisk.read(blockNo,myBuffer);

	char next = freeSpace.getNextBlock();
	
	int added = -1;

	if (!freeSpace.hasFreeSpace())
		return added;

	for (int i=0; i <myBlockSize; i++){
		if (myBuffer->data[i]=='\0'){
			myBuffer->data[i]=next;
			myDisk.write(blockNo,myBuffer);
			added=next;
			break;
		}
	}
	cout << "added: " <<added;
	return added;
}

int getCurrBlock(string file){
 	DiskBlockType *myBuffer = new DiskBlockType(myBlockSize);;
	myDisk.read(directory[file],myBuffer);
	int currentBlock;	
	for (int i=0; i <myBlockSize; i++){
		if (myBuffer->data[i]=='\0'){
			if (i==0)
			       currentBlock = addBlock(file);
	 		else 
				currentBlock = i-1;
			break;
		}
	}
	//else return the last element of the buffer
	currentBlock= myBuffer->data[myBlockSize-1];
	return currentBlock;
}

void writeFile (string file, char* data){

	bool memfull = false;
	if (directory.find (file)==directory.end())
		cout << FILENOTFOUND;


	else{
	//get index block 
		DiskBlockType *myBuffer = new DiskBlockType(myBlockSize);
		int currentBlock = getCurrBlock(file);
		 myDisk.read(currentBlock,myBuffer);
		if (currentBlock == -1)
			cout << MEMORYFULLMSG;
		else{
        		DiskBlockType *myBuffer = new DiskBlockType(myBlockSize);
			int index =0;//to traverse data string
			int offset=0;
		
			for (;offset <myBlockSize; offset++){
				if (myBuffer->data[offset]=='.')
					break;
			}
			if (offset=myBlockSize-1){
				offset = 0;
				currentBlock = addBlock(file);
				if (currentBlock!=-1)
					myDisk.read (currentBlock, myBuffer);
				else 
					memfull = true;
			}

			
			while (data[index]!='\0' && !memfull){
		 		while (offset < myBlockSize)
					myBuffer->data[index]=data[index];
				myDisk.write (currentBlock, myBuffer);
				memfull = (currentBlock = addBlock (file) == -1);
				offset=0;
			}

			if (memfull)
				cout << MEMORYFULLMSG;
		}
	
	}
}

int main(int argc, char **argv){
  	DiskBlockType *myBuffer = new DiskBlockType(myBlockSize);
 
	createFile ("filename");
	createFile ("secondfile");
//	addBlock ("filename");

	//cout << endl << addBlock ("filename");

         for (int i=0; i<10; i++) {
		 myDisk.read(i,myBuffer);
    		 // print comtents to the screen
     		cout<<"Block "<<i<<" : ";
     		for (int j=0; j<myBlockSize; j++) cout << myBuffer->data[j];
     			cout<<"\n";
   	}		

}
