/*
 File management system driver and file system module functions
 Author: Raheema Kadwa
 Date: Spring 2019

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

///////////helper functions 
//(I couldn't figure out typecasting in C++
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

//Will create a file and allocate an index block on the disk.
//Will also add file name to directory 
void createFile (string file){
	if (!freeSpace.hasFreeSpace())
		cout << MEMORYFULLMSG;
	else{
		directory [file]= freeSpace.getNextBlock();
		DiskBlockType *myBuffer = new DiskBlockType(myBlockSize, true);
		myBuffer->setIndex (directory[file]);
		myDisk.write (directory[file], myBuffer);
	}
}

//Will delete file and will all of the space allocated to the file 
//This includes index blocks and data blocks. 
//Will also delete file from directory 
void deleteFile (string file){
        if (directory.find (file)==directory.end())
                cout << FILENOTFOUND;
        else{
                DiskBlockType *myBuffer = new DiskBlockType(myBlockSize, true);
		int blockNo = directory[file];
		int temp;
       		myDisk.read(directory[file],myBuffer);
		for (int x=0; x < myBlockSize; x++){
			cout << "x: " << x <<endl;
			if (myBuffer->indeces[x]==NULL){
				temp = myBuffer->getIndex();
                        	myDisk.freeBlock(temp);
				freeSpace.add_node(temp);
				break;
			}	
                	else{
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

//traverses index block and returns the next block to write to
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

/*int Write(in handle, int numchars, char *buffer); // writes numchars
chars that are in buffer to the file with this handle. Returns the number actually written or
-1 if there is an error.*/
int writeFile (string file, int numchars, char* data){
	bool memfull = false;
	int index =0; // to traverse data string
	if (directory.find (file)==directory.end())
		cout << FILENOTFOUND;
	else{
	//get index block 
		DiskBlockType *currentBlock = getCurrBlock(file);
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
		bool endofstring=false;
		while (!memfull){
			while (offset < myBlockSize &&  !endofstring){
				currentBlock->data[offset]=data[index];
				offset++;
				index++;
				endofstring = (data[index]=='\0' || index>=numchars);
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
				return -1;
				cout << MEMORYFULLMSG;
				break;
			}	
		}
	}
	return index;	
}


/*int Read(int handle, int numchars, char *buffer); // reads numchars
from file with this handle and returns these in buffer. Returns the number of characters
actually read (might be less than numchars), or -1 if there is an error (e.g., no such handle)
*/
int readFile(string file, int numchars, char *buffer){
	char *temp;
	int total=0;
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
			while (temp[index]!='\0' && total<numchars){
                                index++;
				total++;
				buffer[total]=temp[index];	
			}
                }
        }

        return total;
}


//Returns the size of the file passed
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

// returns a list of all currently existing file names as stings
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

//Helper function for testing to print contents of disk 
void printDisk(){
  DiskBlockType *myBuffer = new DiskBlockType(myBlockSize, true);
  
  for (int i=0; i<10; i++) {
                 myDisk.read(i,myBuffer);
                 // print contents to the screen
                cout<<"Block "<< myDisk.getBlock(i)->getIndex()<<" : ";
                for (int j=0; j<myBlockSize; j++) {
                        if (myBuffer->indexType)
                                cout << myBuffer->indeces[j] << ", ";
                        else
                                cout << myBuffer ->data[j];
                }
                cout <<endl;
        }
}

int main(int argc, char **argv){
	
	DiskBlockType *myBuffer = new DiskBlockType(myBlockSize, true); 
	createFile ("filename");
	createFile ("secondFile");
	cout << writeFile ("filename", 5, "well the we will see if this works?n");
 	cout << writeFile ("filename", 5, "and maybe it wont");
	writeFile ("secondFile", 10, "&&&&&&&");
	writeFile ("secondFile", 10, "++++flkmdlmgfjndkfjnl");
	writeFile ("filename",  10, "heehe");
	char* buffer = new char[10]; 
	cout << readFile ("filename", 10, buffer);

	//deleteFile ("filename");
	//createFile ("thirdFile");
	//writeFile ("thirsFile", 10,  "another name");
	//myDisk.freeBlock(2);

	List();

	bool exit = false, debug, passed;
	string command; 
	string filename;
	string data;
	char* dataBuffer;
	int bufferSize=-1; 
	cout << endl << endl << "------\nATOS FILE SYSTEM\n-----"<<endl;
	cout << "Commands: CREATE/DELETE/READ/WRITE/LIST/STATS/HELP"<<endl;
	
	 cout << "Debug mode? This will print disk after each command you enter (Y/N):";
         while (command != "Y" && command != "N")
         	cin >> command;
         debug = (command == "Y");
	while (!exit){
		cout << "Please enter a command: "; 
		cin >> command;
		if (command == "EXIT" || command == "exit"){
			cout << "Quitting..."<<endl;
			exit = true;
			break;
		}
		else if (command == "CREATE"){
			cout << "File name: "; 
			cin >> command; 
			createFile(command);
		}
		else if (command == "DELETE"){
			cout << "File name: ";
                        cin >> command;
                        deleteFile(command);
		}
		else if (command == "WRITE"){
			cout << "File name: "; 
			cin >> command; 
			cout << "Data: "; 
			cin >> data; 
			dataBuffer = const_cast<char*>(data.c_str());
			passed = (writeFile (command, myBlockSize,dataBuffer) > 0);
		        if (passed)
				dataBuffer = NULL;	
		}
		else if (command == "READ"){
			cout << "File name: "; 
			cin >> command; 
			while (bufferSize<0){
				cout << "No. bits you would like to read: ";
				cin >> bufferSize; 
			}dataBuffer = new char[bufferSize];
			passed = (readFile (command, myBlockSize, dataBuffer)>=0);
			cout << "passed: " << passed <<endl;	
			if (passed){
				for (int x=0; x < bufferSize; x++)
				cout << dataBuffer[x];
				cout << endl;
				delete dataBuffer;// = NULL;
			}
			bufferSize = -1;
		}
		else if (command == "LIST"){
			List();
		}
		else if (command == "STATS"){
			cout << "File name: ";
                        cin >> command;
                        stats(command);
		}
		else if (command == "HELP"){
		}
		else {
			cout << endl << "Please enter a valid command. Type 'HELP' for instructions."<<endl;
		}

		cout << endl;
		if (debug){
			cout << endl <<endl;
			printDisk();
			cout << endl <<endl;
		}
	}
}	


