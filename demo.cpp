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


int myBlockSize= 10;
int myDiskSize = 10;
linked_list freeSpace(myBlockSize);
DiskProcessType myDisk(myBlockSize,myDiskSize);
std::map<std::string, int> directory;
std::map<std::string, vector<int>> directory2;


char intToChar (int x){
	char characters[] = "0123456789";
	return characters [x];
}

char charToInt (char x){
        switch(x){
		case '0':
			return 0;
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
	}
}

//Strings: 
string MEMORYFULLMSG = "\nDirectory is full. Please delete a file and try again...\n";
string FILENOTFOUND = "\nThe file you entered was not found. Please try again...\n";


void createFile (string file){
	if (!freeSpace.hasFreeSpace())
		cout << MEMORYFULLMSG;
	else{
		directory [file]= freeSpace.getNextBlock();
		DiskBlockType *myBuffer = new DiskBlockType(myBlockSize, true);
		myBuffer->setIndex (directory[file]);
		cout << file << ": " << myBuffer->getIndex() <<endl;
		myDisk.write (directory[file], myBuffer);
	}
}

void deleteFile (string file){
        if (directory.find (file)==directory.end())
                cout << FILENOTFOUND;
        else{
                DiskBlockType *myBuffer = new DiskBlockType(myBlockSize, true);
		int blockNo = directory[file];
		int temp;
       		myDisk.read(directory[file],myBuffer);
		cout << 1 <<endl;
		cout << "Block number: " << blockNo <<endl;
		for (int x=0; x < myBlockSize; x++){
			cout << "x: " << x <<endl;
			if (myBuffer->indeces[x]==NULL){
				cout << "que"<<endl;
				temp = myBuffer->getIndex();
                        	myDisk.freeBlock(temp);
				freeSpace.add_node(temp);
				break;
			}	
                	else{
				cout << "here" <<endl;
				cout << "Temp: " << temp << endl;
				temp=myBuffer->indeces[x]->getIndex();
			 	myDisk.freeBlock(temp);
			 	freeSpace.add_node(temp);
			}
			directory.erase(file);
		}
	}
}

//helper function that returns next block to be added to the index block
int addBlock(string file){
	int blockNo = directory[file];
	
	DiskBlockType *myBuffer = new DiskBlockType(myBlockSize, true);

	if (myBuffer->indeces[0]!=NULL)
		myDisk.read(blockNo,myBuffer);
	
	if (myDisk.getBlock(blockNo))
		myDisk.read(blockNo,myBuffer);

	int next_int = freeSpace.getNextBlock();
	char next = intToChar(next_int);
	int added = -1;

	if (next_int==-1)
		return added;

	for (int i=0; i <myBlockSize; i++){
		if (myBuffer->indeces[i]==0){
			myBuffer->indeces[i]= myDisk.getBlock(next_int);
			myBuffer->indeces[i]->setIndex(next_int);
			myDisk.write(blockNo,myBuffer);
			added=next_int;
			break;
		}
	}
	return added;
}

DiskBlockType* getCurrBlock(string file){
 	DiskBlockType *myBuffer = new DiskBlockType(myBlockSize,true);
	int blockNo = directory[file];
	
	myDisk.read(directory[file],myBuffer);
	
	for (int i=0; i <myBlockSize; i++){
		if (myBuffer->indeces[i]==0){
			if (i==0){
			        int next = addBlock(file);
				return myDisk.getBlock(next);
			}
	 		else {
				return myBuffer->indeces[i-1];
			}break;	
		}
	}
	return myBuffer->indeces[myBlockSize-1];
}

void writeFile (string file, char* data){
	bool memfull = false;
	if (directory.find (file)==directory.end())
		cout << FILENOTFOUND;
	else{
	//get index block 
		DiskBlockType *currentBlock = getCurrBlock(file);
		int index =0;//to traverse data string
		int offset=0;
		int nextBlock;
		for (;offset <myBlockSize; offset++){
			if (currentBlock->data[offset]=='\0')
				break;
		}		
		if (offset==myBlockSize-1){
			offset = 0;
			nextBlock = addBlock(file);
			if (nextBlock!=-1)
				myDisk.read(nextBlock, currentBlock);
			else 
				memfull = true;
		}
		bool endofstring;
		while (!memfull){
			while (offset < myBlockSize &&  data[index]!='\0'){
				currentBlock->data[offset]=data[index];
				offset++;
				index++;

				endofstring = (data[index]=='\0');
			}

			if (endofstring)
				break;

			nextBlock = addBlock (file);
			memfull = (nextBlock == -1);
			if (!memfull){
				currentBlock = myDisk.getBlock (nextBlock);
				offset=0;
				continue;	
			}
			else{
				cout << MEMORYFULLMSG;
				break;
			}	
		}
	}	
}


int stats (string file){
	int fileSize=0; 
	char *temp;

	if (directory.find (file)==directory.end()){
                cout << FILENOTFOUND;
		return -1;
	}

	DiskBlockType *myBuffer = new DiskBlockType(myBlockSize,true);
        int blockNo = directory[file];
        myDisk.read(directory[file],myBuffer);
	
	for (int x=0; x < myBlockSize; x++){
		if (myBuffer->indeces[x]==NULL)
			break;
		else{
			temp=myBuffer->indeces[x]->data; 
			int index=0;
			while (temp[index]!='\0'){
				index++;	
				fileSize++;
			}	
		}
	}

	return fileSize; 

}

vector<string> List(){
	std::map<std::string, int>::iterator it = directory.begin();
	vector<string> list;
	cout << "\n-------Directory-------\n";
	while (it != directory.end()){
		list.push_back(it->first);
		cout << it->first<<": " << stats(it->first) << endl;
		it++;	
	}
	cout << "------------------------\n"<<endl;
	return list;
}

int main(int argc, char **argv){
  	DiskBlockType *myBuffer = new DiskBlockType(myBlockSize, true); 
	createFile ("filename");
	createFile ("secondFile");
	writeFile ("filename","well the we will see if this works?n");
 	writeFile ("filename", "and maybe it wont");
	writeFile ("secondFile", "&&&&&&&");
	writeFile ("secondFile", "++++flkmdlmgfjndkfjnl");
	writeFile ("filename",  "heehe");

	deleteFile ("filename");
	createFile ("thirsFile");
	writeFile ("thirsFile", "another name");
	//myDisk.freeBlock(2);

	List();

	for (int i=0; i<10; i++) {
		 myDisk.read(i,myBuffer);
    		 // print comtents to the screen
     		cout<<"Block "<< myDisk.getBlock(i)->getIndex()<<" : ";
     		for (int j=0; j<myBlockSize; j++) {
			if (myBuffer->indexType)
				cout << myBuffer->indeces[j] << ", ";
			else 
				cout << myBuffer ->data[j];
		}
		cout <<endl;
   	}

	cout << "End of file/..." <<endl;	
}	


