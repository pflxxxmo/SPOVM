#include "lib.h"
#include "FileSystem.h"

void createFileSystem() {
	FILE *fileSystem;
	FILE *fileSystemSectorsControl;
	int diskSize = 0;
	int flag = 0;
	char buff = '0';

	cout << "Create Disk" << endl;
	while (diskSize <= 0 || diskSize > MAX_DISK_SIZE) {
		cout << endl << "Enter disk size : ";
		cin >> diskSize;
		if (diskSize <= 0 || diskSize > MAX_DISK_SIZE) {
			cout << "Error!";
		}
	}
	cout << endl;

	fileSystem = fopen(DISK_NAME, "w");
	fileSystemSectorsControl = fopen(DISK_SECTORS_CONTROL, "w");
	fprintf(fileSystemSectorsControl, "%d", diskSize);

	FileSystemItem root("root");
	root.writeDataInTreeFile();

	fclose(fileSystem);
	fclose(fileSystemSectorsControl);
}

void loadFolderFromFile(FILE *fileSystemTree, FileSystemItem **current, int depth) {
	int buffForBranches = 0;
	int buffForSectorsSize = 0;
	int buffForFileOrFolder = 0;
	int buffForStartSector = 0;

	if (depth == 0) {
		while (!feof(fileSystemTree)) {
			char *buffForItemName = new char[MAX_NAME_SIZE];
			fscanf(fileSystemTree, "%s ", buffForItemName);
			fscanf(fileSystemTree, "%d ", &buffForFileOrFolder);
			if (buffForFileOrFolder == _FOLDER) {
				current[depth]->createNewFolder(buffForItemName);
				fscanf(fileSystemTree, "%d ", &buffForBranches);
				if (buffForBranches > 0) {
					depth++;
					current[depth] = current[depth - 1]->moveToNextFolder(buffForItemName);
					for (int i = 0; i < buffForBranches; i++) {
						loadFolderFromFile(fileSystemTree, current, depth);
					}
					current[depth] = NULL;
					depth--;
				}
			}
			else {
				fscanf(fileSystemTree, "%d ", &buffForSectorsSize);
				fscanf(fileSystemTree, "%d ", &buffForStartSector);
				current[depth]->createNewFile(buffForItemName, buffForSectorsSize, buffForStartSector);
			}
			delete[] buffForItemName;
		}
	}
	else {
		char *buffForItemName = new char[MAX_NAME_SIZE];
		fscanf(fileSystemTree, "%s ", buffForItemName);
		fscanf(fileSystemTree, "%d ", &buffForFileOrFolder);
		if (buffForFileOrFolder == _FOLDER) {
			current[depth]->createNewFolder(buffForItemName);
			fscanf(fileSystemTree, "%d ", &buffForBranches);
			if (buffForBranches > 0) {
				depth++;
				current[depth] = current[depth - 1]->moveToNextFolder(buffForItemName);
				for (int i = 0; i < buffForBranches; i++) {
					loadFolderFromFile(fileSystemTree, current, depth);
				}
				current[depth] = NULL;
				depth--;
			}
		}
		else {
			fscanf(fileSystemTree, "%d ", &buffForSectorsSize);
			fscanf(fileSystemTree, "%d ", &buffForStartSector);
			current[depth]->createNewFile(buffForItemName, buffForSectorsSize, buffForStartSector);
		}
		delete[] buffForItemName;
	}
}

void loadDataFormFile(FileSystemItem **current) {
	int depth = 0;
	FILE *fileSystemTree;
	fileSystemTree = fopen(DISK_TREE_NAME, "r");

	char *buffForRootItemName = new char[MAX_NAME_SIZE];
	int buffForFileOrFolder = 0;
	int buffForRootBranches = 0;

	fscanf(fileSystemTree, "%s ", buffForRootItemName);
	fscanf(fileSystemTree, "%d ", &buffForFileOrFolder);
	fscanf(fileSystemTree, "%d ", &buffForRootBranches);

	loadFolderFromFile(fileSystemTree, current, depth);

	delete[] buffForRootItemName;
	fclose(fileSystemTree);
}

void help() {
	cout << endl << "                     H  E  L  P                                   " << endl;
	cout << endl << "------------------------------------------------------------------";
	cout << endl << "|  ls             -- show list of files                          |";
	cout << endl << "|  cd 'name'      -- move to folder                              |";
	cout << endl << "|  pf             -- move to previous folder                     |";
	cout << endl << "|  touch 'name'   -- create new file in this directory           |";
	cout << endl << "|  mkdir 'name'   -- create new folder in this directory         |";
	cout << endl << "|  rm 'name'      -- delete file or folder                       |";
	cout << endl << "|  exit           -- close programm                              |";
	cout << endl << "------------------------------------------------------------------" << endl << endl;
}

void openFileSystem() {
	bool flag = false;
	int depth = 0;

	FileSystemItem *current[MAX_FOLDERS_DEPTH];
	current[depth] = new FileSystemItem("root");
	for (int i = 1; i < MAX_FOLDERS_DEPTH; i++) {
		current[i] = new FileSystemItem();
		current[i] = NULL;
	}
	loadDataFormFile(current);

	while (1) {
		flag = false;
		if (depth == 0) {
			cout << "home:\\>";
		}
		else {
			cout << "home:\\";
			for (int i = 1; i <= depth; i++) {
				cout << current[i]->getItemName();
				cout << "\\";
			}
			cout << ">";
		}

		char *inputCommand = new char[CHAR_BUFFER_SIZE];
		cin >> inputCommand;

		for (int i = 0; i < NUMBER_OF_COMMANDS; i++) {
			if (strcmp(inputCommand, commands[i]) == 0) {
				flag = true;
				switch (i) {
				case COMMAND_HELP: {
					help();
					break;
				}
				case COMMAND_SHOW_DIRECTORY_STRUCTURE: {
					current[depth]->seeDirectoryList();
					break;
				}
				case COMMAND_MOVE_TO_NEXT_FOLDER: {
					char *inputSecondArg = new char[CHAR_BUFFER_SIZE];
					cin >> inputSecondArg;
					depth++;
					current[depth] = current[depth - 1]->moveToNextFolder(inputSecondArg);
					if (current[depth] == NULL) {
						depth--;
					}
					break;
				}
				case COMMAND_MOVE_TO_PREVIOUS_FOLDER: {
					if (depth > 0) {
						current[depth] = NULL;
						depth--;
					}
					else {
						cout << "	Error!" << endl << endl;
					}
					break;
				}
				case COMMAND_EXIT: {
					delete[] inputCommand;
					return;
				}
				case COMMAND_CREATE_NEW_FILE: {
					char *inputSecondArg = new char[CHAR_BUFFER_SIZE];
					cin >> inputSecondArg;
					current[depth]->createNewFile(inputSecondArg, 0, 0);
					delete[] inputSecondArg;
					remove(DISK_TREE_NAME);
					current[0]->writeDataInTreeFile();
					break;
				}
				case COMMAND_CREATE_NEW_FOLDER: {
					char *inputSecondArg = new char[CHAR_BUFFER_SIZE];
					cin >> inputSecondArg;
					current[depth]->createNewFolder(inputSecondArg);
					delete[] inputSecondArg;
					remove(DISK_TREE_NAME);
					current[0]->writeDataInTreeFile();
					break;
				}
				case COMMAND_DELETE_FILE_OR_FOLDER: {
					char *inputSecondArg = new char[CHAR_BUFFER_SIZE];
					cin >> inputSecondArg;
					current[depth]->deleteFileOrFolder(inputSecondArg);
					delete[] inputSecondArg;
					remove(DISK_TREE_NAME);

					int start = 0;
					int size = 0;
					FILE *tempFile;
					tempFile = fopen("temp2", "r");
					if (tempFile) {
						fscanf(tempFile, "%d ", &size);
						fscanf(tempFile, "%d ", &start);
						fclose(tempFile);
						remove("temp2");
						current[0]->defragmentation(start, size);
					}

					current[0]->writeDataInTreeFile();
					break;
					break;
				}
				default:
					break;
				}
				break;
			}
		}

		if (flag == false) {
			cout << endl << "Error! Use 'help'." << endl << endl;
		}

		delete[] inputCommand;
		while (cin.get() != '\n');
	}

	for (int i = 1; i < MAX_FOLDERS_DEPTH; i++) {
		delete current[i];
	}
	delete current[depth];
}

int main() {
	FILE *fileSystem;
	FILE *fileSystemTree;
	FILE *fileSystemSectorsControl;

	fileSystem = fopen(DISK_NAME, "r+b");
	fileSystemTree = fopen(DISK_TREE_NAME, "r+b");
	fileSystemSectorsControl = fopen(DISK_SECTORS_CONTROL, "r+b");

	if (!fileSystem || !fileSystemTree || !fileSystemSectorsControl) {
		createFileSystem();
	}

	if (fileSystem) {
		fclose(fileSystem);
	}
	if (fileSystemTree) {
		fclose(fileSystemTree);
	}
	if (fileSystemSectorsControl) {
		fclose(fileSystemSectorsControl);
	}

	openFileSystem();

	return 0;
}