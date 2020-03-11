/*
Name: Aaron Lim Cong Kai
Student ID: 5985171
Done on: 12/08/2019
*/

#include <iostream>
#include <vector>
#include <math.h>
#include <ctime>	//to set the time for randomo
#include <cstdlib>	//for srand and rand
#include <fstream>	//for ifstream and reading of file
#include <string>
#include <stdio.h>	//for strtok
#include <sstream>	//for stringstream
#include <map>		//for multimap

struct peer;

typedef peer *peerPtr;

struct peer{
	int uniqueID;
	std::multimap<int, std::string> key_data;
	int *fingerTable;
	peerPtr next;
	peerPtr prev;
};

//For initializing the chord
//Note* if initialized again, refresh the chord
//Use pointer
//Init(N,...)
void Init(int n, int &calledTime, peerPtr &chord, int &size, int &no, bool &can);

//for adding new peer
//NewPeer(ID,..)
//“PEER <ID> ADDED”, where <ID> is the ID of the peer just inserted.
void NewPeer(peerPtr &chord, int unique, int size, int n, int &no);

//for removing peer
//RemovePeer(ID,...)
//“PEER <ID> REMOVED”.
void RemovePeer(peerPtr &chord, int unique, int n, int &no);

//For looking up of peers, hash key
//Resolve(key,...)
/*
For example, if a peer with ID = 1
initiated a search and the IDs of the peers visited during 
the search were 7 and 12 then this function will print
1>7>12
*/
//peerPtr Resolve(peerPtr &chord, int unique, int n);

//For computing hash key for given data(ID of peer)
//Hash(string)
/*
Ensure that the returned value is within [0;2n) 
by using an operation which connects the end points such as in the
sequence (…,2n-2,2n-1,0,1,2,...).
*/
int Hash(std::string str, int n);

//For storing of peer with hash
//Store(string,...)
//Use of Hash(string)
//“INSERTED <string> (key=<key_value>) AT “ followed by the ID value of the
//peer at which the data item is stored.
void Store(peerPtr &chord, std::string str, int n, int no);

//for deleting of peer and ID
//“REMOVED <string> (key=<key_value>) FROM “ followed by the ID value
//Delete(string,...)
void Delete(peerPtr &chord, std::string str, int n, int no);

//for printing the element
//May need to call function, FindKey()
//“DATA AT NODE <MYID:>”, where <MYID> is the ID
//Print(key,...)
void Print(peerPtr &chord, int unique, int n, int no);

//for reading the file
//to delimit # and the thing after
//Read(filename)
void Read(char *fileName);

//check for hash
std::string checkHash(std::string line);

//seperate two string
void seperator(std::string line, std::string &first, std::string &second);

//check if is which command
int checkCommand(std::string n);

//convert string to int
template <typename T> void convertStringToInt(std::string, T &);

//Power function
template <typename T> T pow(T n);

//creating chord
void chordCreation(peerPtr &chord, int n);

//deleting chord
void deleteChord(peerPtr &chord, int no);

//finger table initialize
void ftInit(peerPtr &curr, peerPtr &chord, int n, int no);

//peer initialize
peerPtr peerInit(int unique, int n);

//void establishKey(peerPtr &chord, int n);

//print current chord
void printChord(peerPtr &chord, int no);

peerPtr Resolve(peerPtr &chord, int unique, int n, int no);

void establishChordFT(peerPtr &chord, int n, int no);

void restartKeyData(peerPtr &chord, int no);

int main(int argc, char *argv[]){
	char *fileName = argv[1];
	
	Read(fileName);
	return 0;
}

//For initializing the chord
//Note* if initialized again, refresh the chord
//Use pointer
//Init(N,...)
void Init(int n, int &calledTime, peerPtr &chord, int &size, int &no, bool &can){
	if(calledTime > 0){
		deleteChord(chord, no);
		std::cout << "Chord deleted" << std::endl;
	}
	if(n >= 1 && n < 32){
		size = pow(n);
		std::cout << "Name: Aaron Lim Cong Kai\nStudent ID: 5985171" << std::endl;
		chordCreation(chord, n);
		no = 1;
		can = true;
	}
	else{
		calledTime = 0;
		can = false;
		std::cout << "n must be 1 <= n < 32, current n: " << n << std::endl;
	}
}

//for adding new peer
//NewPeer(ID,..)
//“PEER <ID> ADDED”, where <ID> is the ID of the peer just inserted.
void NewPeer(peerPtr &chord, int unique, int size, int n, int &no){
	bool ok = true;
	bool fakeOk = true;
	peerPtr tempPtr = peerInit(unique, n);
	peerPtr curr = chord;
	
	if(unique < 0 || unique > size){
		fakeOk = false;
	}

	if(no == 1 && fakeOk){
		if(chord -> uniqueID < tempPtr -> uniqueID){
			curr -> next = tempPtr;
			tempPtr -> prev = curr;
			tempPtr -> next = curr;
			curr -> prev = tempPtr;
			chord = curr;
			ok = false;
			
		}
		else if(chord -> uniqueID > tempPtr -> uniqueID){
			curr -> prev = tempPtr;
			tempPtr -> next = curr;
			tempPtr -> prev = curr;
			curr -> next = tempPtr;
			chord = tempPtr;
			ok = false;
		}
	}
	else if(fakeOk){
		curr = chord;
		//std::cout << chord -> prev -> uniqueID << std::endl;
		if(chord -> uniqueID > tempPtr -> uniqueID){
			curr = chord;
			chord = tempPtr;
			chord -> next = curr;
			chord -> prev = curr -> prev;
			curr -> prev = chord;
			chord -> prev -> next = chord;
			//std::cout << chord -> prev -> uniqueID << std::endl;
			ok = false;
		}
		if(ok){
			//last curr
			curr = chord;
			while(curr -> next != chord){
				curr = curr -> next;
			}
			if(curr -> uniqueID < tempPtr -> uniqueID){
				curr -> next = tempPtr;
				tempPtr -> prev = curr;
				tempPtr -> next = chord;
				chord -> prev = tempPtr;
				ok = false;
			}
		}
		if(ok){
			curr = chord;
			while(ok){
				if(curr -> uniqueID > tempPtr -> uniqueID && curr -> prev -> uniqueID < tempPtr -> uniqueID){
					curr -> prev -> next = tempPtr;
					tempPtr -> prev = curr -> prev;
					tempPtr -> next = curr;
					curr -> prev = tempPtr;
					ok = false;
				}
				curr = curr -> next;
				if(curr == chord && ok){
					ok = false;
					fakeOk = false;
				}
			}
		}
	}
	
	if(!ok && fakeOk){
		std::cout << "PEER " << tempPtr -> uniqueID << " ADDED" << std::endl;
		no++;
	}
	else{
		std::cout << "PEER " << tempPtr -> uniqueID << " IS NOT ADDED" << std::endl;
	}
	
	std::cout << "total number of chord: " << no << std::endl;
	restartKeyData(chord, no);
	//establish first finger table
	establishChordFT(chord , n, no);
	if(no != 1){
		//update finger table for others
		curr = chord -> next;
		while(curr != chord){
			std::cout << "curr uniqueID: " << curr -> uniqueID << std::endl;
			ftInit(curr, chord, n, no);
			/*
			for(int i = 0; i < n; i++){
				std::cout << curr -> uniqueID << " > " << curr -> fingerTable[i] << std::endl;
			}
			*/
			curr = curr -> next;
		}
	}
	printChord(chord, no);
}

void restartKeyData(peerPtr &chord, int no){
	peerPtr curr = chord;
	std::multimap<int, std::string>::iterator itr;
	std::multimap<int, std::string> restart;
	bool check = true;
	bool alltrue = true;

	if(no == 1){
		alltrue = false;
		check = false;
	}
	if(check){
		curr = chord -> next;
	}
	while(curr != chord && check){
		for(itr = curr -> key_data.begin(); itr != curr -> key_data.end(); itr++){
			restart.insert(std::pair<int, std::string>(itr->first, itr->second));
		}
		if(!(curr -> key_data.empty())){
			curr -> key_data.erase(curr -> key_data.begin(), curr -> key_data.end());
		}
		//std::cout << curr -> uniqueID << std::endl;
		curr = curr -> next;
		//std::cout << curr -> uniqueID << std::endl;
	}
	if(check){
		for(itr = chord -> key_data.begin(); itr != chord -> key_data.end(); itr++){
			restart.insert(std::pair<int, std::string>(itr->first, itr->second));
		}
		if(!(chord -> key_data.empty())){
			chord -> key_data.erase(chord -> key_data.begin(), chord -> key_data.end());
		}
	}

	curr = chord -> next;
	while(curr != chord && alltrue){
		for(itr = restart.begin(); itr != restart.end(); itr){
			check = true;
			if(curr -> uniqueID >= itr -> first && curr -> prev -> uniqueID < itr -> first){
				curr -> key_data.insert(std::pair<int, std::string>(itr->first, itr->second));
				restart.erase(itr);
			}
			if(check){
				itr++;
			}
		}
		curr = curr -> next;
	}
	if(alltrue){
		for(itr = restart.begin(); itr != restart.end(); itr++){
			chord -> key_data.insert(std::pair<int, std::string>(itr->first, itr->second));
			restart.erase(itr);
		}
	}
}

void establishChordFT(peerPtr &chord, int n, int no){
	peerPtr curr;
	int temp;	
	int j;
	bool ok = true;
	std::cout << "curr uniqueID: " << chord -> uniqueID << std::endl;
	//std::cout << chord -> uniqueID << std::endl;
	if(no == 1){
		for(int i = 0; i < n; i++){
			chord -> fingerTable[i] = chord -> uniqueID;
			std::cout << chord -> uniqueID << " > " << chord -> fingerTable[i] << std::endl;
		}
		ok = false;
	}
	if(ok){
		for(int i = 0; i < n; i++){
			ok = true;
			curr = chord -> next;
			temp = chord -> uniqueID + pow(i);
			std::cout << chord -> uniqueID;
			while(curr != chord && ok){
				if(curr -> prev -> uniqueID < temp && curr -> uniqueID >= temp){
					chord -> fingerTable[i] = curr -> uniqueID;
					std::cout << " > " << curr -> uniqueID;
					ok = false;
					break;
				}
				if(ok){
					std::cout << " > " << curr -> uniqueID;
					curr = curr -> next;
				}
			}
			if(curr == chord && ok){
				chord -> fingerTable[i] = chord -> uniqueID;
				std::cout << " > " << chord -> uniqueID;
			}
			std::cout << std::endl;
		}
	}
}

//for removing peer
//RemovePeer(ID,...)
//“PEER <ID> REMOVED”.
//std::multimap<int, std::string> key_data;
void RemovePeer(peerPtr &chord, int unique, int n, int &no){
	std::multimap<int, std::string>::iterator itr;
	peerPtr curr = chord;
	peerPtr temp;
	bool check = true;
	bool fakeOk = true;
	int size = pow(n);
	if(unique < 0 || unique > size){
		fakeOk = false;
	}
	
	if(no == 1 && chord -> uniqueID == unique && fakeOk){
		delete [] chord -> fingerTable;
		if(!(chord -> key_data.empty())){
			chord -> key_data.erase(chord -> key_data.begin(), chord -> key_data.end());
		}
		delete chord;
		std::cout << "Last of the peers removed. CHORD terminated" << std::endl;
		exit(-1);
	}
	
	if(no == 1 && fakeOk){
		fakeOk = false;
	}
	
	else if(no == 2 && fakeOk){
		if(chord -> uniqueID == unique){
			temp = chord;
			chord = chord -> next;
			chord -> prev = NULL;
			chord -> next = NULL;
			temp -> prev = NULL;
			temp -> next = NULL;
			for(itr = temp -> key_data.begin(); itr != temp -> key_data.end(); itr++){
				chord -> key_data.insert(std::pair<int, std::string>(itr->first, itr->second));
			}
			if(!(temp -> key_data.empty())){
				temp -> key_data.erase(temp -> key_data.begin(), temp -> key_data.end());
			}
			
			delete [] temp -> fingerTable;
			delete temp;
			check = false;
		}
		else if(chord -> next -> uniqueID == unique){
			temp = chord -> next;
			chord -> next = NULL;
			chord -> prev = NULL;

			temp -> prev = NULL;
			temp -> next = NULL;
			for(itr = temp -> key_data.begin(); itr != temp -> key_data.end(); itr++){
				chord -> key_data.insert(std::pair<int, std::string>(itr->first, itr->second));
			}
			if(!(temp -> key_data.empty())){
				temp -> key_data.erase(temp -> key_data.begin(), temp -> key_data.end());
			}
			delete [] temp -> fingerTable;
			delete temp;
			check = false;
		}
		else{
			fakeOk = false;
		}
	}
	else if(no >= 3 && fakeOk){
		curr = chord;
		if(chord -> uniqueID == unique){
			temp = curr;
			curr = curr -> next;
			temp -> prev -> next = curr;
			curr -> prev = temp -> prev;
			chord = curr;
			temp -> next = NULL;
			temp -> prev = NULL;
			for(itr = temp -> key_data.begin(); itr != temp -> key_data.end(); itr++){
				chord -> key_data.insert(std::pair<int, std::string>(itr -> first, itr->second));
			}
			if(!(temp -> key_data.empty())){
				temp -> key_data.erase(temp -> key_data.begin(), temp -> key_data.end());
			}
			delete [] temp -> fingerTable;
			delete temp;
			check = false;
		}
		if(check){
			curr = chord;
			if(curr -> uniqueID == unique){
				temp = curr;
				curr = curr -> prev;
				curr -> next = chord;
				chord -> prev = curr;
				temp -> next = NULL;
				temp -> prev = NULL;
				for(itr = temp -> key_data.begin(); itr != temp -> key_data.end(); itr++){
					curr -> key_data.insert(std::pair<int, std::string>(itr->first, itr->second));
				}
				if(!(temp -> key_data.empty())){
					temp -> key_data.erase(temp -> key_data.begin(), temp -> key_data.end());
				}
			
				delete [] temp -> fingerTable;
				delete temp;
				check = false;
			}
		}
		if(check){
			curr = chord -> next;
			while(curr != chord){
				if(curr -> uniqueID == unique){
					temp = curr;
					curr = curr -> next;
					curr -> prev = temp -> prev;
					curr -> prev -> next = curr;
					temp -> next = NULL;
					temp -> prev = NULL;
					for(itr = temp -> key_data.begin(); itr != temp -> key_data.end(); itr++){
						curr -> key_data.insert(std::pair<int, std::string>(itr->first, itr->second));
					}
					if(!(temp -> key_data.empty())){
						temp -> key_data.erase(temp -> key_data.begin(), temp -> key_data.end());
					}
			
					delete [] temp -> fingerTable;
					delete temp;
					check = false;
				}
				else{
					curr = curr -> next;
				}
			}
			if(curr == chord && check){
				fakeOk = false;
			}
		}
	}

	if(check && !fakeOk){
		std::cout << "The unique ID " << unique << " is not found in the chord" << std::endl;
	}
	else{
		no--;
		std::cout << "PEER " << unique << " REMOVED" << std::endl;
	}	
	std::cout << "total number of chord: " << no << std::endl;

	restartKeyData(chord, no);

	//establish first finger table
	establishChordFT(chord, n, no);
	
	if(no != 1){
		//update finger table for others
		curr = chord -> next;
		while(curr != chord){
			std::cout << "curr uniqueID: " << curr -> uniqueID << std::endl;
			ftInit(curr, chord, n, no);
			/*
			for(int i = 0; i < n; i++){
				std::cout << curr -> uniqueID << " > " << curr -> fingerTable[i] << std::endl;
			}
			*/
			curr = curr -> next;
		}
	}

	printChord(chord, no);
}

//For computing hash key for given data(ID of peer)
//Hash(string)
/*
Ensure that the returned value is within [0;2n) 
by using an operation which connects the end points such as in the
sequence (…,2n-2,2n-1,0,1,2,...).
*/
int Hash(std::string str, int n){
	unsigned int key = 0;

	for(int i = 0; i < str.size(); i++){
		key = (((key << 5) + key) ^ str[i]) % pow(n);
	}
	
	return key;
}

//For storing of peer with hash
//Store(string,...)
//Use of Hash(string)
//“INSERTED <string> (key=<key_value>) AT “ followed by the ID value of the
//peer at which the data item is stored.
void Store(peerPtr &chord, std::string str, int n, int no){
	str = str.substr(0, str.size() - 1);
	int unique = Hash(str, n);
	bool check = true;
	peerPtr curr = chord;
	std::multimap<int, std::string>::iterator itr;

	for(int i = 0; i < no; i++){
		if(curr -> key_data.find(unique) -> first == unique){			
			for(itr = curr -> key_data.begin(); itr != curr -> key_data.end(); itr++){
				if(itr -> second == str){
					check = false;
					break;
				}
			}
		}
		curr = curr -> next;
	}
	if(check){
		curr = Resolve(chord, unique, n, no);
	
		curr -> key_data.insert(std::pair<int, std::string> (unique, str));
		std::cout << "INSERTED " << str << " (key = " << unique << ") AT " << curr -> uniqueID << std::endl;
	}
	else{
		std::cout << "Text already added, will be ignored" << std::endl;
	}
}

//for deleting of peer and ID
//“REMOVED <string> (key=<key_value>) FROM “ followed by the ID value
//Delete(string,...)
void Delete(peerPtr &chord, std::string str, int n, int no){
	str = str.substr(0, str.size() -1);
	int unique = Hash(str, n);
	peerPtr curr = chord;
	bool check = false;
	std::multimap<int, std::string>::iterator itr;

	for(int i = 0; i < no; i++){
		for(itr = curr -> key_data.begin(); itr != curr -> key_data.end(); itr++){
			if(itr -> second == str){
				check = true;
				break;
			}
		}
		curr = curr -> next;
	}
	if(check){
		curr = Resolve(chord, unique, n, no);
		std::multimap<int, std::string>::iterator itr;
		int i = 0;	
		itr = curr -> key_data.find(unique);
		curr -> key_data.erase(itr);
	
		std::cout << "REMOVED " << str << " (key =  " << unique << ") AT " << curr -> uniqueID << std::endl;
	}
	else{
		std::cout << "Non-existance text" << std::endl;
	}
}

//for printing the element
//May need to call function, FindKey()
//“DATA AT NODE <MYID:>”, where <MYID> is the ID
//Print(key,...)
void Print(peerPtr &chord, int unique, int n, int no){
	peerPtr curr = chord -> next;
	bool check = true;
	bool have = true;	

	if(no == 1){
		if(chord -> uniqueID != unique){
			have = false;
		}
		if(have){
			curr = chord;
			std::cout << "DATA AT INDEX NODE " << unique << ":" << std::endl;
			std::multimap <int, std::string>::iterator itr;
			for(itr = curr -> key_data.begin(); itr != curr -> key_data.end(); itr++){
				std::cout << itr -> second << std::endl;
			}
			std::cout << "FINGER TABLE OF NODE " << unique << ":" << std::endl;
			for(int i = 0; i < n; i++){
				std::cout << curr -> fingerTable[i] << " ";
			}
			std::cout << std::endl;
		}
	}
	
	else if(no >= 2){
		have = false;
		for(int i = 0; i < no; i++){
			if(curr -> uniqueID == unique){
				have = true;
			}
			curr = curr -> next;
		}
		if(have){
			curr = chord -> next;
			while(curr != chord){
				if(curr -> uniqueID == unique){
					check = false;
					std::cout << "DATA AT INDEX NODE " << unique << ":" << std::endl;
					std::multimap <int, std::string>::iterator itr;
					for(itr = curr -> key_data.begin(); itr != curr -> key_data.end(); itr++){
						std::cout << itr -> second << std::endl;
					}
					std::cout << "FINGER TABLE OF NODE " << unique << ":" << std::endl;
					for(int i = 0; i < n; i++){
						std::cout << curr -> fingerTable[i] << " ";
					}
					std::cout << std::endl;
					break;
				}
				curr = curr -> next;
			}
			if(curr == chord && check){
				check = false;
				std::cout << "DATA AT INDEX NODE " << unique << ":" << std::endl;
				std::multimap <int, std::string>::iterator itr;
				for(itr = curr -> key_data.begin(); itr != curr -> key_data.end(); itr++){
					std::cout << itr -> second << std::endl;
				}
				std::cout << "FINGER TABLE OF NODE " << unique << ":" << std::endl;
				for(int i = 0; i < n; i++){
					std::cout << curr -> fingerTable[i] << " ";
				}
				std::cout << std::endl;
			}
		}
		if(check && !have){
			std::cout << unique << " is not a succ(p)" << std::endl;
		}
	}
}

//for reading the file
//to delimit # and the thing after
//Read(filename)
void Read(char *fileName){
	std::string delimiter = "#";
	std::ifstream infile;

	//need to configure to open dynamic file
	infile.open(fileName);
	
	if(!infile.is_open()){
		std::cout << "File does not exist" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::string fullLine;
	std::string first;
	std::string second;	
	int check;
	int calledTime = 0;
	int n;
	peerPtr chord = NULL;
	int unique;
	int size;
	int no = 0;
	bool can = false;
	while(infile.good()){
		while(getline(infile, fullLine)){
			fullLine = checkHash(fullLine);
			seperator(fullLine, first, second);
			check =  checkCommand(first);
			switch(check){
				case 0: convertStringToInt(second, n);
					Init(n, calledTime, chord, size, no, can);
					if(can){
						calledTime++;
					}
					break;
				case 1:	if(can){
						convertStringToInt(second, unique);
						NewPeer(chord, unique, size, n, no);
					}
					else{
						std::cout << "Wrong value for init, must be within 1 <= n < 31" <<  std::endl;
					}
					break;
				case 2:	if(can){
						convertStringToInt(second, unique);
						RemovePeer(chord, unique, n, no);
					}
					else{
						std::cout << "Wrong value for init, must be within 1 <= n < 31" <<  std::endl;
					}
					break;
				case 3: if(can){
						Store(chord, second, n, no);
					}
					else{
						std::cout << "Wrong value for init, must be within 1 <= n < 31" <<  std::endl;
					}
					break;
				case 4:	if(can){
						convertStringToInt(second, unique);
						Print(chord, unique, n, no);
					}
					else{
						std::cout << "Wrong value for init, must be within 1 <= n < 31" <<  std::endl;
					}
					break;
				case 5:	if(can){
						Delete(chord, second, n, no);
					}
					else{
						std::cout << "Wrong value for init, must be within 1 <= n < 31" <<  std::endl;
					}
					break;
				default:;
			}
		}
	}
	infile.close();
}

//check for hash
std::string checkHash(std::string line){
	char h;
	bool ok = true;
	std::string returnValue;
	for(int i = 0; i <line.size(); i++){
		h = line[i];
		if(h == '#' || h == '\t'){
			ok =false;
			break;
		}
		returnValue += h;
	}
	return returnValue + " ";
}

//seperate two string
void seperator(std::string fullLine, std::string &first, std::string &second){
	second = "";
	std::string delimiter = " ";
	size_t pos = 0;

	int i = 0;
	std::string token;
	while((pos = fullLine.find(delimiter)) != std::string::npos){
		token = fullLine.substr(0, pos);
		fullLine.erase(0,pos + delimiter.length());
		if(i == 0){
			first = token;
		}
		else{
			second += token + " ";
		}
		i++;
	}
}

//check if is which command
int checkCommand(std::string n){
	int returnValue;
	if(!n.compare("init")){
		returnValue = 0;
	}
	else if(!n.compare("newpeer")){
		returnValue = 1;
	}
	else if(!n.compare("removepeer")){
		returnValue = 2;
	}
	else if(!n.compare("store")){
		returnValue = 3;
	}
	else if(!n.compare("print")){
		returnValue = 4;
	}
	else if(!n.compare("delete")){
		returnValue = 5;
	}
	else{
		returnValue = 6;
	}
	return returnValue;
}

template <typename T> void convertStringToInt(std::string str, T &n){
	std::istringstream iss(str);
	iss >> n;
	if(iss.fail()){
		std::cout << "The value is not an integer" << std::endl;		
		exit(EXIT_FAILURE);
	}
}

//Power function
template <typename T> T pow(T n){
	int size;
	size = (int)pow(2, n);
	return size;
}

//creating chord
void chordCreation(peerPtr &chord, int n){
	std::cout << "Initializing chord..." << std::endl;
	peerPtr temp = peerInit(0, n);
	temp -> next = NULL;
	temp -> prev = NULL;
	for(int i = 0; i < n; i++){
		temp -> fingerTable[i] = temp -> uniqueID;
		std::cout << temp -> uniqueID << " > " << temp -> fingerTable[i] << std::endl;
	}
	
	chord = temp;

	std::cout << "Initialzation successful" << std::endl;
}

//deleting chord
void deleteChord(peerPtr &chord, int no){
	if(no != 1){
		peerPtr temp;
		peerPtr curr = chord -> next;
		while(curr != chord){
			temp = curr;
			curr = curr -> next;
			curr -> prev = temp -> prev;
			delete [] temp -> fingerTable;
			temp -> key_data.clear();
			//temp -> key_data.shrink_to_fit();
			delete temp;
		}
	}
	delete [] chord -> fingerTable;
	chord -> key_data.clear();
	//chord -> key_data.shrink_to_fit();
	delete chord;
	chord = NULL;
}

//finger table initialize
void ftInit(peerPtr &curr, peerPtr &chord, int n, int no){
	int temp;
	peerPtr tempPtr;					
	bool check = true;
	for(int i = 0; i < n; i++){
		check = true;
		//succ(p + 2^(i-1)
		//succ(uniqueID + 2^(i-1)
		temp = pow(i);						//establish the power
		temp = (curr -> uniqueID + temp) % pow(n);					//establish the finger table on i		
		//std::cout << temp << std::endl;		
		tempPtr = chord;						//initialized tempPtr to 0 everytime
		tempPtr = Resolve(chord, temp, n, no);
		curr -> fingerTable[i] = tempPtr -> uniqueID;
	}
}

peerPtr Resolve(peerPtr &chord, int unique, int n, int no){
	peerPtr curr;
	bool found = true;
	int j;
	std::cout << chord -> uniqueID;
	j = 0;
	
	if(no == 1){
		std::cout << " > " << chord -> uniqueID << std::endl;
		curr = chord;
		return curr;
	}

	for(int i = n-1; i >= 0; i--){
		curr = chord;
		while(chord -> fingerTable[i] == chord -> uniqueID){
			i--;
		}
	
		if(chord -> fingerTable[i] >= unique){
			while(curr -> uniqueID != chord -> fingerTable[i]){
				curr = curr -> next;
			}
			if(curr -> prev -> uniqueID < unique && curr -> uniqueID >= unique){
				std::cout << " > " << curr -> uniqueID;
				found = false;
				break;
			}
		}
		else if(chord -> fingerTable[i] < unique){
			while(curr -> uniqueID != chord -> fingerTable[i]){
				curr = curr -> next;
			}
			break;
		}
	}
	if(found){
		while(curr != chord && found){
			std::cout << " > " << curr -> uniqueID;
			if(curr -> prev -> uniqueID < unique && curr -> uniqueID >= unique){
				found = false;
			}
			else{
				curr = curr -> next;
			}
		}
	}
	if(found){
		j = 0;
		curr = chord;
		std::cout << " > " << chord -> uniqueID;
	}
	std::cout << std::endl;
	return curr;
}

//peer initialize
peerPtr peerInit(int unique, int n){
	peerPtr temp = new peer;
	temp -> uniqueID = unique;
	temp -> fingerTable = new int[n];

	return temp;
}

//print chord
void printChord(peerPtr &chord, int no){
	if(no == 1){
		std::cout << "Current peers: " << chord -> uniqueID << " > " << chord -> uniqueID << std::endl;
	}
	else{
		std::cout << "Current peers: ";
		peerPtr curr = chord -> next;
		std::cout << chord -> uniqueID << " > ";
		while(curr != chord){
			std::cout << curr -> uniqueID << " > ";
			curr = curr -> next;
		}
		std::cout << chord -> uniqueID << std::endl;
	}
}
