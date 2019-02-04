#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define bool int
#define true 0==0
#define false !true
#define INITIAL_BUFFER_SIZE 16

char* transpilePiece(FILE* srcFile, double arrayShift);
bool containsActualCode(char* code);

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("Usage: %s <file> [-s / --start-index <start index>]\n", argv[0]);
    return -1;
  }

  double startIndex = 1.0;
  char* inputFiles[argc];
  int numInputFiles = 0;

  // Check arguments.
  for (int i = 1; i < argc; i++) {
    if (strncmp(argv[i], "--start-index", 13) == 0 || strncmp(argv[i], "-s", 2) == 0) {
      // A start index is provided.
      if (i == argc - 1) {
        perror("No start index provided.");
        exit(-1);
      }
      startIndex = strtod(argv[++i], NULL);
      if (startIndex == 0) {
        printf("Error: Attempted to use 0 as a start index.\n");
        exit(-1);
      }
    } else {
      // A file is provided. Make sure it ends with .c1.
      char* fileName = argv[i];
      if (strcmp(fileName + strlen(fileName) - 3, ".c1") != 0) {
        printf("Error: %s doesn't look like a .c1 file\n", fileName);
        return -1;
      }
      inputFiles[numInputFiles] = fileName;
      numInputFiles++;
    }
  }

  for (int i = 0; i < numInputFiles; i++) {
    char* srcFileName = inputFiles[i];
    FILE* srcFile;
    FILE* intermediaryFile;
    char dstFileName[strlen(srcFileName)];
    char command[strlen(srcFileName) + 64];

    // Try to open the source file.
    srcFile = fopen(srcFileName, "r");
    if (srcFile == NULL) {
      perror("fopen");
      return -1;
    }

    // Try to open the intermediary file.
    intermediaryFile = fopen(".tmp.c", "w");
    if (intermediaryFile == NULL) {
      perror("fopen");
      return -1;
    }

    // Get the destination file name (remove .c1).
    strcpy(dstFileName, srcFileName);
    dstFileName[strlen(dstFileName) - 3] = '\0';

    // Transpile to regular C.
    char* transpiledCode = transpilePiece(srcFile, startIndex);
    for (int i = 0; transpiledCode[i] != 0; i++) {
      fputc(transpiledCode[i], intermediaryFile);
    }
    fclose(intermediaryFile);
    free(transpiledCode);

    // Compile to an executable.
    sprintf(command, "gcc -o %s .tmp.c", dstFileName);
    system(command);
  }

  return 0;
}

// Transpiles a piece of code. Returns true if there was some code to transpile.
// If there's not enough RAM, returns NULL.
char* transpilePiece(FILE* srcFile, double arrayShift) {
  int bufferSize = INITIAL_BUFFER_SIZE;
  char* transpiledCode = malloc(sizeof(char) * bufferSize);
  *transpiledCode = '\0';

  for (char ch = getc(srcFile); ch >= 0; ch = getc(srcFile)) {
    char *newCode;

    if (ch == '[') {
      // Maybe, an array access started. The sub-code already consumes the
      // closing bracket.
      char* subCode = transpilePiece(srcFile, arrayShift);
      if (containsActualCode(subCode)) {
        newCode = malloc(sizeof(subCode) + sizeof(char) * 32);
        if (newCode == NULL) return NULL;
        sprintf(newCode, "[(int) (((double) %s) - %f)]", subCode, arrayShift);
      } else {
        newCode = malloc(sizeof(char) * 3);
        if (newCode == NULL) return NULL;
        strcpy(newCode, "[]");
      }
    } else if (ch == ']') {
      // The array access ended. Return to the parent function, returning the
      // transpiled code after the loop.
      break;
    } else {
      // A single code unit was read.
      newCode = malloc(sizeof(char) * 2);
      if (newCode == NULL) return NULL;
      *newCode = ch;
      *(newCode + 1) = '\0';
    }

    // Reallocate more memory, if needed.
    int newSize = strlen(transpiledCode) + strlen(newCode);
    if (newSize > bufferSize) {
      transpiledCode = realloc(transpiledCode, newSize * 2);
      if (transpiledCode == NULL) return NULL;
    }
    strcat(transpiledCode, newCode);
    free(newCode);
  }

  return transpiledCode;
}

// Checks whether the given code contains actual meaningful code (expressions
// or statements).
bool containsActualCode(char* code) {
  char ch;
  for (int i = 0; code[i] != '\0', ch = code[i]; i++) {
    if (ch != ' ' && ch != '\n' && ch != '\r' && ch != '\t') {
      return true;
    }
  }
  return false;
}
