//
// Chicago Crime Lookup via hashing.
//
// <<James Warda>>
// <<Platforms: Linux, OSX, Windows; Programmed in Windows 10>>
// U. of Illinois, Chicago
// CS251, Spring 2017
// Project #05
//
#define _CRT_SECURE_NO_WARNINGS
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>

//Debug Global Scope Values
int NUMBEROFCRIMES = 0;
long CRIMESFILESIZE = 0;
int	TABLESIZE = 0;
int COLLISIONS = 0;

//--------------------------------------------------
//typeDef
typedef struct Case
{
	char CaseNumber[9];
	char IUCRCode[5];
	char DateAndTime[30];
	int District;
	char ArrestMade;
	int hashIndex;
	struct Case *next;
} Case;

//-----------------------------------------------------
//function prototypes
char *getFileName();
long getFileSizeInBytes(char *filename);
void readCrimeDataInToTable(char *crimeDataFileName, Case **Table);
Case **createTable(int size);
void insertNode(Case *caseNode, Case **Table);
int hash(char *caseNumber, int size);
Case *findCaseNumber(char caseNumber[9], Case **table);
char ** createAreaTable();
void readAreasData(char **areaTable, char *areaFileName);
char *trueOrFalse(char x);
int checkIfValid(char caseNumber[255]);

//------------------------------------------------------
//Main
int main() {
	//declare variables
	int fileSize = 0;
	char caseNumber[255];

	// get filenames from the user/stdin:
	char *crimeDataFileName = getFileName();
	char *AreasFileName = getFileName();

	//dynamicly create the table
	fileSize = getFileSizeInBytes(crimeDataFileName);
	CRIMESFILESIZE = fileSize;
	fileSize = fileSize / 50;
	fileSize *= 13;
	TABLESIZE = fileSize;
	Case **crimeTable = createTable(fileSize);
	char **areaTable = createAreaTable();

	//read in all values in to hashtable
	readCrimeDataInToTable(crimeDataFileName, crimeTable);
	readAreasData(areaTable, AreasFileName);

	printf("HIII %d\n", sizeof(Case));
	printf("HII %d\n", sizeof(Case*));

	//output debug info
	printf(">> Crimes file: %ld bytes\n", CRIMESFILESIZE);
	printf(">> # crimes:    %d\n", NUMBEROFCRIMES);
	printf(">> HT size:     %d\n", TABLESIZE);
	printf(">> Collisions:  %d\n\n", COLLISIONS);

	//get case number to search
	printf("Enter Case Number> ");
	fgets(caseNumber, 255, stdin);
	caseNumber[strcspn(caseNumber, "\r\n")] = '\0';  // strip EOL(s) char at end:

	while (strlen(caseNumber) > 0) {
		if (checkIfValid(caseNumber) == 1) {
			Case *foundNode = findCaseNumber(caseNumber, crimeTable);
			if (foundNode != NULL) {
				printf(">> hash index: %d <<\n", foundNode->hashIndex);
				printf("%s:\n", caseNumber);
				printf("  date/time: %s\n", foundNode->DateAndTime);
				printf("  city area: %d => %s\n", foundNode->District, areaTable[foundNode->District]);
				printf("  IURC code: %s\n", foundNode->IUCRCode);
				printf("  arrested:  %s\n\n", trueOrFalse(foundNode->ArrestMade));
			}
			else {
				printf("** Case not found...\n");
			}
		}
		else
		{
			printf("** invalid case #, try again...\n");
		}
		printf("Enter Case Number> ");
		fgets(caseNumber, 255, stdin);
		caseNumber[strcspn(caseNumber, "\r\n")] = '\0';  // strip EOL(s) char at end:
	}

	system("pause");
	return 0;
}

//----------------------------------------------------
//function checks if user input is valid
int checkIfValid(char caseNumber[255]) {
	int numberCheck = 0;
	numberCheck = atoi(&caseNumber[1]);
	//check length
	if (strlen(caseNumber) != 8) {
		return 0;
	}
	//check if the first letter is R
	else if (caseNumber[0] != 'R') {
		return 0;
	}
	//check rest of values
	else if ((isdigit(caseNumber[1])) && (isdigit(caseNumber[2])) && (isalpha(caseNumber[3])) &&
		(isdigit(caseNumber[4])) && (isdigit(caseNumber[5])) && (isdigit(caseNumber[6])) && (isdigit(caseNumber[7]))) {
		//check if number is from 0 to 77
		if (numberCheck > 77 || numberCheck < 0) {
			return 0;
		}
		else
		{
			return 1;
		}
		}
	
	return 0;
}

//----------------------------------------------------
//returns true if arrest made, otherwise false
char *trueOrFalse(char x) {
	char *True = "true";
	char *False = "false";
	if (x == 'F') {
		return False;
	}
	return True;
}

//-----------------------------------------------------
//function looks case number up in hash table
Case *findCaseNumber(char caseNumber[9], Case **table) {
	int hashIndex = hash(caseNumber, TABLESIZE);

	//if does not exist
	if (table[hashIndex] == NULL) {
		return NULL;
	}

	//else if index has no collisons in it
	else if (strcmp(table[hashIndex]->CaseNumber, caseNumber) == 0) {
		return table[hashIndex];
	}
	
	//else travers collisons
	else
	{
		Case *head = table[hashIndex];
		while (head != NULL)
		{
			if (strcmp(head->CaseNumber, caseNumber) == 0) {
				return head;
			}
			head = head->next;
		}
	}
	return NULL;//not found
}

//-----------------------------------------------------
//function reads Areas data in to table
void readAreasData(char **areaTable, char *areaFileName) {
	FILE *infile = fopen(areaFileName, "r");

	//declare variables
	char buffer[255];
	char *token;
	char delem[2] = ",";
	int index = 0;
	int i = 0;
	char *newName;

	//prime file for reading data
	fgets(buffer, 255, (FILE*)infile);
	//traverse file and input values in to array
	while (fgets(buffer, 255, (FILE*)infile) != NULL) {
		buffer[strcspn(buffer, "\r\n")] = '\0';  // strip EOL(s) char at end:
		token = strtok(buffer, delem);
		index = atoi(token);
		token = strtok(NULL, delem);
		newName = (char *)malloc(sizeof(char) * strlen(token));
		strcpy(newName, token);
		areaTable[index] = newName;
	}
}

//------------------------------------------------------
//function reads file data in to hashtable
void readCrimeDataInToTable(char *crimeDataFileName, Case **Table) {
	FILE *infile = fopen(crimeDataFileName, "r");

	//declare variables
	char buffer[255];
	char *token;
	char delem[2] = ",";
	int counter = 0;
	int hashindex = 0;

	char CaseNumber[9];
	char IUCRCode[5];
	char DateAndTime[30];
	int District;
	char ArrestMade;

	Case *caseNode;

	//prime file for reading data
	fgets(buffer, 255, (FILE*)infile);

	//begin parsing data 
	while (fgets(buffer, 255, (FILE*)infile) != NULL) {
		//grab first token
		token = strtok(buffer, delem);

		while (token != NULL) {
			//grab case number
			if (counter == 0) {
				strcpy(CaseNumber, token);
			}//end for if

			 //grab IUCR
			else if (counter == 1) {
				strcpy(IUCRCode, token);
			}//end for else

			 //grab date and time
			else if (counter == 2) {
				strcpy(DateAndTime, token);
			}

			//grab district
			else if (counter == 4) {
				District = atoi(token);
			}

			//grab arrest
			else if (counter == 6) {
				ArrestMade = *token;
			}
			counter++;
			token = strtok(NULL, delem);
		}//end for while
		
		//insert data in to Node
		caseNode = (Case *)malloc(sizeof(Case));
		if (caseNode == NULL)
		{
			printf("** Error: malloc failed to allocate memory for newNode");
			exit(-1);
		}
		caseNode->ArrestMade = ArrestMade;
		strcpy(caseNode->CaseNumber, CaseNumber);
		strcpy(caseNode->DateAndTime, DateAndTime);
		caseNode->District = District;
		strcpy(caseNode->IUCRCode, IUCRCode);
		caseNode->next = NULL;

		//hash and insert in to table
		insertNode(caseNode, Table);
		counter = 0;
	}//end for while
	fclose(infile);
}

//-------------------------------------------
//function inserts node in to table
void insertNode(Case *caseNode, Case **Table) {
	int hashIndex = 0;
	hashIndex = hash(caseNode->CaseNumber, TABLESIZE);
	caseNode->hashIndex = hashIndex;

	//check for collisions
	if (Table[hashIndex] == NULL) {
		Table[hashIndex] = caseNode;
	}

	//else traverse the list and insert at the end
	else
	{
		Case *head = Table[hashIndex];
		Case *prev = NULL;
		//go to end of list
		while (head != NULL){
			prev = head;
			head = head->next;
		}
		//inesrt
		prev->next = caseNode;
		COLLISIONS++;
	}

	NUMBEROFCRIMES++;
}

//--------------------------------------------
//fnuction Creaets Hash
int hash(char *caseNumber, int size) {
	int i = 0;
	int hashValue = 0;
	char tempNumber[30];
	char value[4];

	value[0] = caseNumber[6];
	value[1] = caseNumber[7];

	strcpy(tempNumber, caseNumber);

	//transfer last four numbers
	for (i = 0; i < 4; i++) {
		tempNumber[i] = caseNumber[i + 4];
	}

	tempNumber[4] = caseNumber[1];
	tempNumber[5] = caseNumber[2];

	hashValue = atoi(tempNumber);
	hashValue += caseNumber[3];
	hashValue += atoi(&value);
	
	return hashValue% size;
}

//--------------------------------------------
//function creates table for districts
char ** createAreaTable() {
	char **A = (char **)malloc(80 * sizeof(char *));
	if (A == NULL)
	{
		printf("** Error: malloc failed to allocate hash table (%d elements).\n", 80);
		exit(-1);
	}
	for (int i = 0; i < 80; ++i)
	{
		A[i] = NULL;
	}
	return A;
}

//---------------------------------------------
//function returns allocated array set to null
Case **createTable(int size)
{
	Case **A = (Case **)malloc(size * sizeof(Case *));

	if (A == NULL)
	{
		printf("** Error: malloc failed to allocate hash table (%d elements).\n", size);
		exit(-1);
	}

	for (int i = 0; i < size; ++i)
	{
		A[i] = NULL;
	}

	return A;
}

//---------------------------------------------------
//function gets file size for dynamic allocation
long getFileSizeInBytes(char *filename)
{
	FILE *file = fopen(filename, "r");
	if (file == NULL)
		return -1;
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fclose(file);
	return size;
}

//
// getFileName: <Imported from previous Project Joe Hummel>
//
// Inputs a filename from the keyboard, make sure the file can be
// opened, and returns the filename if so.  If the file cannot be 
// opened, an error message is output and the program is exited.
//
char *getFileName()
{
	char filename[512];
	int  fnsize = sizeof(filename) / sizeof(filename[0]);

	// input filename from the keyboard:
	fgets(filename, fnsize, stdin);
	filename[strcspn(filename, "\r\n")] = '\0';  // strip EOL char(s):

												 // make sure filename exists and can be opened:
	FILE *infile = fopen(filename, "r");
	if (infile == NULL)
	{
		printf("**Error: unable to open '%s'\n\n", filename);
		exit(-1);
	}

	fclose(infile);

	// duplicate and return filename:
	char *s = (char *)malloc((strlen(filename) + 1) * sizeof(char));
	strcpy(s, filename);

	return s;
}