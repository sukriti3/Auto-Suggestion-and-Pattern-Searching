#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Include time.h for measuring time

#define ALPHABET_SIZE 256
#define SUCCESS 0
#define FAILURE 1

int flag = 0; // Flag to check if pattern is found

// Struct to store the occurrence details
typedef struct {
    int lineNumber;  // Line number where the pattern is found
    int startIndex;  // Start index of the pattern in the line
} Occurrence;

// Node for the suffix tree structure
typedef struct SuffixTreeNode {
    struct SuffixTreeNode* children[ALPHABET_SIZE]; // Array of child nodes for each possible character
    Occurrence occurrenceList[100]; // Array to store occurrences of the pattern
    int occurrenceCount;            // Count of occurrences in this node
} SuffixTreeNode;

// Struct to represent the suffix tree
typedef struct {
    SuffixTreeNode* rootNode; // Root node of the tree
} SuffixTree;

// Function to create a new suffix tree node
SuffixTreeNode* createSuffixTreeNode() {
    SuffixTreeNode* newNode = (SuffixTreeNode*)malloc(sizeof(SuffixTreeNode));
    // Initialize all children to NULL and occurrence count to 0
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        newNode->children[i] = NULL;
    }
    newNode->occurrenceCount = 0;
    return newNode;
}

// Function to initialize the suffix tree
SuffixTree* initializeSuffixTree() {
    SuffixTree* suffixTree = (SuffixTree*)malloc(sizeof(SuffixTree));
    suffixTree->rootNode = createSuffixTreeNode();
    return suffixTree;
}

// Function to add a suffix to the tree
void addSuffixToTree(SuffixTree* suffixTree, const char* suffix, int lineNum, int startIndex) {
    SuffixTreeNode* currentNode = suffixTree->rootNode;
    for (int i = startIndex; suffix[i] != '\0'; i++) {
        char character = suffix[i];
        // Create a new node if the character path doesn't exist
        if (currentNode->children[character] == NULL) {
            currentNode->children[character] = createSuffixTreeNode();
        }
        currentNode = currentNode->children[character];

        // Add occurrence info if occurrence list isn't full
        if (currentNode->occurrenceCount < 100) {
            currentNode->occurrenceList[currentNode->occurrenceCount].lineNumber = lineNum + 1;
            currentNode->occurrenceList[currentNode->occurrenceCount].startIndex = startIndex;
            currentNode->occurrenceCount++;
        }
    }
}

// Function to build the suffix tree from the lines of text
void buildSuffixTreeFromLines(SuffixTree* suffixTree, char** lines, int lineCount) {
    // Add all suffixes of each line to the tree
    for (int lineNum = 0; lineNum < lineCount; lineNum++) {
        const char* line = lines[lineNum];
        int length = strlen(line);
        for (int i = 0; i < length; i++) {
            addSuffixToTree(suffixTree, line, lineNum, i);
        }
    }
}

// Function to search for a pattern in the suffix tree
void findPatternInTree(SuffixTreeNode* node, const char* pattern, int index, char** lines) {
    if (node == NULL) return;

    int cnt = 0; // Count of pattern occurrences
    // If the entire pattern has been matched
    if (pattern[index] == '\0') {
        if (node->occurrenceCount > 0) {
            printf("Pattern found!\n");
            for (int i = 0; i < node->occurrenceCount; i++) {
                int lineNum = node->occurrenceList[i].lineNumber;
                int startIndex = node->occurrenceList[i].startIndex;

                // Find the word containing the pattern
                const char* line = lines[lineNum - 1];
                int wordStart = startIndex;
                while (wordStart > 0 && line[wordStart - 1] != ' ') wordStart--; // Move to start of the word
                int wordEnd = startIndex;
                while (line[wordEnd] != '\0' && line[wordEnd] != ' ') wordEnd++; // Move to end of the word

                // Extract the word
                char foundWord[256];
                strncpy(foundWord, line + wordStart, wordEnd - wordStart);
                foundWord[wordEnd - wordStart] = '\0';

                // Output the position of the pattern and the word
                printf("  Found at Line: %d, Position in line: %d, Word: '%s'\n", lineNum, startIndex + 1, foundWord);
                cnt++;
            }
            flag = 1; // Set flag to indicate pattern found
        }
        printf("The Frequency of the pattern is: %d\n", cnt);
        return;
    }

    // Recursive call to check the next character in the pattern
    char character = pattern[index];
    findPatternInTree(node->children[character], pattern, index + 1, lines);
}

// Function to release memory used by the suffix tree
void releaseSuffixTree(SuffixTreeNode* node) {
    // Recursively free each child node
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i] != NULL) {
            releaseSuffixTree(node->children[i]);
        }
    }
    free(node); // Free current node
}

// Function to load lines from a file into an array
char** loadLinesFromFile(const char* filename, int* lineCount) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open file");
        return NULL;
    }

    char** lines = malloc(100 * sizeof(char*)); // Allocate memory for lines
    *lineCount = 0;
    char line[256];

    // Read lines from the file
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // Remove newline character
        lines[*lineCount] = strdup(line); // Duplicate line to store in array
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
        return FAILURE; // Exit if file can't be loaded
    }

    SuffixTree* suffixTree = initializeSuffixTree(); // Initialize the suffix tree
    buildSuffixTreeFromLines(suffixTree, lines, lineCount); // Build the tree from lines

    char pattern[256];
    printf("Enter the pattern to search: ");
    scanf("%255s", pattern);

    // Start time measurement for pattern search
    clock_t start_time = clock();

    // Search for pattern in the tree
    findPatternInTree(suffixTree->rootNode, pattern, 0, lines);

    // End time measurement for pattern search
    clock_t end_time = clock();

    // Calculate and display the elapsed time in milliseconds
    double time_taken = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000;
    printf("Time taken for search: %.2f ms\n", time_taken);

    // Free allocated memory for lines and suffix tree
    for (int i = 0; i < lineCount; i++) {
        free(lines[i]);
    }
    free(lines);
    releaseSuffixTree(suffixTree->rootNode);
    free(suffixTree);

    if (flag == 0) {
        printf("Pattern is not found!\n");
    }
    return SUCCESS;
}



















