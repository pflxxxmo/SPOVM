#pragma once
#include "lib.h"

class FileSystemItem {
private:
	char itemName[MAX_NAME_SIZE];
	bool fileOrFolder;
	int sectorsSize;
	int startSectors;
	int branches;
	FileSystemItem *nextFileOrFolder[MAX_ITEMS_IN_FOLDER];

public:
	FileSystemItem();
	FileSystemItem(char*, int, int);
	FileSystemItem(char*);
	~FileSystemItem();

	void writeDataInTreeFile();
	void seeDirectoryList();
	void createNewFile(char*, int, int);
	void createNewFolder(char*);
	void deleteFileOrFolder(char*);
	void defragmentation(int, int);
	FileSystemItem *moveToNextFolder(char*);
	char *getItemName();
};
