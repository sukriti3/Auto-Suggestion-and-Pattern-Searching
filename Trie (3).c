#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // For measuring time

#define ALPHABET_SIZE 256
#define MAX_OCCURRENCES 100

// Struct to store occurrence details
typedef struct {
    int lineNumber;
    int startIndex;
} Occurrence;

// Node for the trie structure
typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    Occurrence occurrenceList[MAX_OCCURRENCES];
    int occurrenceCount;
} TrieNode;

// Struct to represent the trie
typedef struct {
    TrieNode* root;
} Trie;

// Function to create a new trie node
TrieNode* createTrieNode() {
    TrieNode* newNode = (TrieNode*)malloc(sizeof(TrieNode));
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        newNode->children[i] = NULL;
    }
    newNode->occurrenceCount = 0;
    return newNode;
}

// Function to initialize the trie
Trie* initializeTrie() {
    Trie* trie = (Trie*)malloc(sizeof(Trie));
    trie->root = createTrieNode();
    return trie;
}

// Function to insert a word into the trie
void insertWord(Trie* trie, const char* word, int lineNum, int startIndex) {
    TrieNode* currentNode = trie->root;
    for (int i = 0; word[i] != '\0'; i++) {
        char character = word[i];
        if (currentNode->children[character] == NULL) {
            currentNode->children[character] = createTrieNode();
        }
        currentNode = currentNode->children[character];

        // Add occurrence info if occurrence list isn't full
        if (currentNode->occurrenceCount < MAX_OCCURRENCES) {
            currentNode->occurrenceList[currentNode->occurrenceCount].lineNumber = lineNum + 1;
            currentNode->occurrenceList[currentNode->occurrenceCount].startIndex = startIndex + 1;
            currentNode->occurrenceCount++;
        }
    }
}

// Function to build the trie from the lines of text
void buildTrieFromLines(Trie* trie, char** lines, int lineCount) {
    for (int lineNum = 0; lineNum < lineCount; lineNum++) {
        const char* line = lines[lineNum];
        int length = strlen(line);
        for (int i = 0; i < length; i++) {
            insertWord(trie, line + i, lineNum, i);
        }
    }
}

// Function to search for a pattern in the trie
void searchPatternInTrie(TrieNode* node, const char* pattern, int index) {
    if (node == NULL) return;

    int cnt = 0;
    if (pattern[index] == '\0') {
        if (node->occurrenceCount > 0) {
            printf("Pattern found!\n");
            for (int i = 0; i < node->occurrenceCount; i++) {
                int lineNum = node->occurrenceList[i].lineNumber;
                int startIndex = node->occurrenceList[i].startIndex;
                printf("  Found at Line: %d, Position in line: %d\n", lineNum, startIndex);
                cnt++;
            }
        }
        printf("The Frequency of the pattern is: %d\n", cnt);
        return;
    }

    // Recursive search for the next character in the pattern
    char character = pattern[index];
    searchPatternInTrie(node->children[character], pattern, index + 1);
}

// Function to free the trie memory
void freeTrie(TrieNode* node) {
    if (node == NULL) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        freeTrie(node->children[i]);
    }
    free(node);
}

// Function to load lines from a file into an array
char** loadLinesFromFile(const char* filename, int* lineCount) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open file");
        return NULL;
    }

    char** lines = malloc(100 * sizeof(char*));
    *lineCount = 0;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        lines[*lineCount] = strdup(line);
        (*lineCount)++;
    }
    fclose(file);
    return lines;
}

int main() {
    const char* filename = "sherlock2.txt";
    int lineCount = 0;
    char** lines = loadLinesFromFile(filename, &lineCount);
    if (lines == NULL) {
        return 1;
    }

    Trie* trie = initializeTrie();
    buildTrieFromLines(trie, lines, lineCount);

    char pattern[256];
    printf("Enter the pattern to search: ");
    scanf("%255s", pattern);

    // Start time measurement for pattern search
    clock_t start_time = clock();

    // Search for the pattern in the trie
    searchPatternInTrie(trie->root, pattern, 0);

    // End time measurement
    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000;
    printf("Time taken for search: %.2f ms\n", time_taken);

    // Free memory for lines and trie
    for (int i = 0; i < lineCount; i++) {
        free(lines[i]);
    }
    free(lines);
    freeTrie(trie->root);
    free(trie);

    return 0;
}





















