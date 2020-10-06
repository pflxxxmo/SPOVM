#include "FileSystem.h"

FileSystemItem::FileSystemItem() {}

FileSystemItem::FileSystemItem(char *fileName, int size, int start) {
	strcpy(itemName, fileName);
	fileOrFolder = _FILE;
	sectorsSize = size;
	startSectors = start;
}

FileSystemItem::FileSystemItem(char *folderName) {
	strcpy(itemName, folderName);
	fileOrFolder = _FOLDER;
	branches = 0;
}

FileSystemItem::~FileSystemItem() {
	if (branches > 0) {
		for (int i = 0; i < branches; i++) {
			delete nextFileOrFolder[i];
		}
	}
}

void FileSystemItem::writeDataInTreeFile() {
	FILE *fileSystemTree;
	fileSystemTree = fopen(DISK_TREE_NAME, "a+b");

	fprintf(fileSystemTree, "%s ", itemName);
	fprintf(fileSystemTree, "%d ", fileOrFolder);
	if (fileOrFolder == _FOLDER) {
		fprintf(fileSystemTree, "%d ", branches);
		fclose(fileSystemTree);
		if (branches > 0) {
			for (int i = 0; i < branches; i++) {
				nextFileOrFolder[i]->writeDataInTreeFile();
			}
		}
	}
	else {
		fprintf(fileSystemTree, "%d ", sectorsSize);
		fprintf(fileSystemTree, "%d ", startSectors);
		fclose(fileSystemTree);
	}
}

void FileSystemItem::seeDirectoryList() {
	if (branches == 0) {
		cout << endl << "No files or folders in this directory." << endl << endl;
	}
	else {
		cout << endl << "	NAME" << "			TYPE" << "			SIZE" << endl;
		for (int i = 0; i < branches; i++) {
			cout << endl << "	" << nextFileOrFolder[i]->itemName << "			";
			if (nextFileOrFolder[i]->fileOrFolder == _FILE) {
				cout << "<FILE>			" << nextFileOrFolder[i]->sectorsSize << " bytes";
			}
			else {
				cout << "<FOLDER>";
			}
		}
		cout << endl << endl;
	}
}

void FileSystemItem::createNewFile(char *fileName, int sectors, int start) {
	nextFileOrFolder[branches] = new FileSystemItem(fileName, sectors, start);
	branches++;
}

void FileSystemItem::createNewFolder(char *folderName) {
	nextFileOrFolder[branches] = new FileSystemItem(folderName);
	branches++;
}

void FileSystemItem::defragmentation(int start, int size) {
	if (fileOrFolder == _FOLDER) {
		if (branches > 0) {
			for (int i = 0; i < branches; i++) {
				nextFileOrFolder[i]->defragmentation(start, size);
			}
		}
	}
	else {
		if (startSectors > start) {
			startSectors -= size;
		}
	}
}

void FileSystemItem::deleteFileOrFolder(char *fileOrFolderName) {
	bool flag = false;

	if (branches > 0) {
		for (int i = 0; i < branches; i++) {
			if (strcmp(nextFileOrFolder[i]->itemName, fileOrFolderName) == 0) {
				flag = true;
				if (nextFileOrFolder[i]->fileOrFolder == _FOLDER) {
					if (nextFileOrFolder[i]->branches > 0) {
						cout << endl << "	Is not empty!" << endl << endl;
						return;
					}
				}
				else {
					if (nextFileOrFolder[i]->sectorsSize > 0) {
						cout << "	Defragmentation..." << endl << endl;

						char buff;
						FILE *fileSystem;
						fileSystem = fopen(DISK_NAME, "r+b");
						FILE *tempFile;
						tempFile = fopen("temp", "w+b");
						FILE *tempFile2;
						tempFile2 = fopen("temp2", "w");

						fprintf(tempFile2, "%d ", nextFileOrFolder[i]->sectorsSize);
						fprintf(tempFile2, "%d ", nextFileOrFolder[i]->startSectors);

						for (int j = 0; j < nextFileOrFolder[i]->startSectors; j++) {
							fread(&buff, 1, 1, fileSystem);
							fwrite(&buff, 1, 1, tempFile);
						}
						for (int j = 0; j < nextFileOrFolder[i]->sectorsSize; j++) {
							fread(&buff, 1, 1, fileSystem);
						}
						while (true) {
							if (fread(&buff, 1, 1, fileSystem) == 0) break;
							fwrite(&buff, 1, 1, tempFile);
						}

						fclose(tempFile2);
						fclose(fileSystem);
						fclose(tempFile);

						remove(DISK_NAME);
						rename("temp", DISK_NAME);
					}
				}


				FileSystemItem *temp = NULL;
				for (int j = i; j < branches - 1; j++) {
					temp = nextFileOrFolder[j];
					nextFileOrFolder[j] = nextFileOrFolder[j + 1];
					nextFileOrFolder[j + 1] = temp;
				}
				delete nextFileOrFolder[branches - 1];
				branches--;
				break;
			}
		}
	}

	if (flag == false) {
		cout << endl << "	Incorrect name!" << endl << endl;
	}
}

FileSystemItem *FileSystemItem::moveToNextFolder(char *folderName) {
	bool flag = false;

	if (branches > 0) {
		for (int i = 0; i < branches; i++) {
			if (strcmp(nextFileOrFolder[i]->itemName, folderName) == 0
				&& nextFileOrFolder[i]->fileOrFolder == _FOLDER) {
				return nextFileOrFolder[i];
			}
		}
	}

	if (flag == false) {
		cout << endl << "	Incorrect folder name!" << endl << endl;
	}
	return NULL;
}

char *FileSystemItem::getItemName() {
	return itemName;
}