#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // For measuring execution time

#define MAX_CHAR 256

typedef struct Occurrence {
    int index;  // Index of the word
    int line;   // Line number in the text
    int col;    // Column number in the word
} Occurrence;

typedef struct SuffixTrieNode {
    struct SuffixTrieNode *children[MAX_CHAR];
    Occurrence *occurrences; // Array to store occurrences
    int size;                // Number of occurrences
} SuffixTrieNode;

// Class representing a suffix trie
typedef struct {
    SuffixTrieNode root;
    char **words; // Store words to display them later
    int wordCount; // Number of words
} SuffixTrie;

// Function to create a new node
SuffixTrieNode *createNode() {
    SuffixTrieNode *node = (SuffixTrieNode *)malloc(sizeof(SuffixTrieNode));
    node->occurrences = NULL;
    node->size = 0;
    for (int i = 0; i < MAX_CHAR; i++) {
        node->children[i] = NULL;
    }
    return node;
}

// Function to insert all suffixes of a word into the trie
void insertWord(SuffixTrieNode *node, char *word, int line, int index, int startCol) {
    for (int i = 0; i < strlen(word); i++) {
        SuffixTrieNode *currentNode = node;

        // Insert the suffix starting from index i
        for (int j = i; j < strlen(word); j++) {
            char cIndex = word[j];
            if (currentNode->children[(int)cIndex] == NULL) {
                currentNode->children[(int)cIndex] = createNode();
            }
            currentNode = currentNode->children[(int)cIndex];

            // Store the occurrence with line and column
            currentNode->occurrences = (Occurrence *)realloc(currentNode->occurrences, (currentNode->size + 1) * sizeof(Occurrence));
            currentNode->occurrences[currentNode->size].index = index;
            currentNode->occurrences[currentNode->size].line = line;
            currentNode->occurrences[currentNode->size].col = startCol + j - i + 1; // Correct column number
            currentNode->size++;
        }
    }
}

// Function to search for a pattern in the trie
SuffixTrieNode *search(SuffixTrieNode *node, char *pat, int *size) {
    if (pat[0] == '\0') {
        *size = node->size;
        return node;
    }

    if (node->children[(int)pat[0]] != NULL) {
        return search(node->children[(int)pat[0]], pat + 1, size);
    } else {
        return NULL;
    }
}

// Function to search for a pattern in the suffix trie
void searchPattern(SuffixTrie *trie, char *pat) {
    int resultSize;
    SuffixTrieNode *resultNode = search(&trie->root, pat, &resultSize);
    int totalOccurrences = 0; // To count total occurrences

    // If the pattern is found, display all occurrences
    if (resultNode != NULL) {
        printf("Occurrences of pattern \"%s\":\n", pat);
        for (int i = 0; i < resultNode->size; i++) {
            Occurrence occ = resultNode->occurrences[i];
            printf("Word: \"%s\", Line: %d, Column: %d\n", trie->words[occ.index], occ.line, occ.col);
            totalOccurrences++;
        }
        printf("Total occurrences: %d\n", totalOccurrences);
    } else {
        printf("Pattern not found\n");
    }
}

int main() {
    // Open the input text file "sherlock2.txt"
    FILE *newfile = fopen("sherlock2.txt", "r");
    char *s1 = NULL;
    size_t len = 0;

    // Create a suffix trie
    SuffixTrie trie;
    trie.root = *createNode();
    trie.words = NULL;
    trie.wordCount = 0;

    if (newfile != NULL) {
        int lineNum = 1; // Start at line 1
        // Read the entire file line by line
        while (getline(&s1, &len, newfile) != -1) {
            // Split the line into words
            char *token = strtok(s1, " \n");
            int wordIndex = 0;
            int startCol = 0; // Track the starting column for the current line

            while (token != NULL) {
                // Store the current word
                trie.words = (char **)realloc(trie.words, (trie.wordCount + 1) * sizeof(char *));
                trie.words[trie.wordCount] = strdup(token);

                // Insert the word into the trie with its line and starting column
                insertWord(&trie.root, token, lineNum, trie.wordCount, startCol);

                // Update total word count and word index
                trie.wordCount++;
                wordIndex++;

                // Move to the next token
                token = strtok(NULL, " \n");
                startCol += strlen(trie.words[trie.wordCount - 1]) + 1; // Update startCol
            }
            lineNum++; // Increment line number after each line
        }
        fclose(newfile);
    }

    // Get user input for the pattern to search
    char s2[100];
    printf("Enter pattern to search: ");
    fgets(s2, 100, stdin);
    s2[strcspn(s2, "\n")] = 0; // Remove newline character

    // Start the timer for pattern search
    clock_t start = clock();

    // Search for the pattern in the suffix trie and display the results
    searchPattern(&trie, s2);

    // Stop the timer and calculate the time taken
    clock_t end = clock();
    double time_taken = ((double)(end - start) / CLOCKS_PER_SEC) * 1000; // Time in milliseconds
    printf("Execution time: %.2f ms\n", time_taken);

    // Free memory
    for (int i = 0; i < trie.wordCount; i++) {
        free(trie.words[i]);
    }
    free(trie.words);
    free(s1);

    return 0;
}
















