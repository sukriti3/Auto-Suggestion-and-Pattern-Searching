#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Include time.h for clock()

#define NO_OF_CHARS 256

// Global variables
long long *v1 = NULL;  // Array to store line-ending positions in the text file
int count_fa = 0;      // Count of total occurrences of the prefix
int v1_size = 0;       // Number of lines in the file

// Function to get the next state for the finite automaton
int getNextState(char *pat, int M, int state, int x) {
    if (state < M && x == pat[state])
        return state + 1;

    int ns, i;
    for (ns = state; ns > 0; ns--) {
        if (pat[ns - 1] == x) {
            for (i = 0; i < ns - 1; i++)
                if (pat[i] != pat[state - ns + 1 + i])
                    break;
            if (i == ns - 1)
                return ns;
        }
    }
    return 0;
}

// Function to construct the transition function table (finite automaton)
void computeTF(char *pat, int M, int TF[][NO_OF_CHARS]) {
    int state, x;
    for (state = 0; state <= M; ++state)
        for (x = 0; x < NO_OF_CHARS; ++x)
            TF[state][x] = getNextState(pat, M, state, x);
}

// Function to search for occurrences of a pattern in the text
void search(char *pat, char *txt) {
    int M = strlen(pat);  // Length of the pattern
    int N = strlen(txt);  // Length of the text

    int TF[M + 1][NO_OF_CHARS];
    computeTF(pat, M, TF);  // Build the transition function

    int i, state = 0;
    for (i = 0; i < N; i++) {
        state = TF[state][(unsigned char)txt[i]];  // Update state based on current character

        // If we've reached the accepting state (match found)
        if (state == M) {
            // Identify the start of the word
            int start = i - M + 1;
            while (start > 0 && txt[start - 1] != ' ')
                start--;

            // Identify the end of the word
            int end = i;
            while (end < N && txt[end] != ' ')
                end++;

            // Extract the word that contains the matched pattern
            int word_len = end - start;
            char *word = (char *)malloc(word_len + 1);
            if (!word) {
                printf("Memory allocation failed.\n");
                return;
            }
            strncpy(word, txt + start, word_len);
            word[word_len] = '\0';

            // Find the line number and position within that line
            long long it = 0;
            while (it < v1_size && v1[it] <= start)
                it++;

            int c;
            if (it == 0) {
                c = start;
            } else {
                c = start - v1[it - 1];
            }
            if (it == 0) {
                c++;
            }

            count_fa++;
            printf("Found '%s' at line: %lld position: %d\n", word, it + 1, c);
            free(word);
        }
    }
}

int main() {
    FILE *newfile = fopen("sherlock.txt", "r");
    if (newfile == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }

    char *s1 = NULL; // Holds the complete file content
    size_t s1_len = 0;
    size_t buffer_size = 1024;
    char *buffer = (char *)malloc(buffer_size);

    if (!buffer) {
        printf("Memory allocation failed.\n");
        fclose(newfile);
        return 1;
    }

    // Read the file line by line
    while (fgets(buffer, buffer_size, newfile)) {
        size_t line_len = strlen(buffer);
        s1 = realloc(s1, s1_len + line_len + 2); // +2 for space and null terminator
        if (!s1) {
            printf("Memory allocation failed.\n");
            free(buffer);
            fclose(newfile);
            return 1;
        }

        // Append the line to s1
        memcpy(s1 + s1_len, buffer, line_len);
        s1_len += line_len;
        s1[s1_len] = ' ';
        s1_len++;
        s1[s1_len] = '\0';

        // Store the cumulative length of each line to determine line breaks
        v1 = realloc(v1, (v1_size + 1) * sizeof(long long));
        if (!v1) {
            printf("Memory allocation failed.\n");
            free(s1);
            free(buffer);
            fclose(newfile);
            return 1;
        }
        if (v1_size == 0) {
            v1[v1_size] = line_len;
        } else {
            v1[v1_size] = v1[v1_size - 1] + line_len + 1;
        }
        v1_size++;
    }
    fclose(newfile);
    free(buffer);

    printf("Enter prefix to search: ");
    char s2[256];
    scanf("%s", s2);

    // Measure the time taken for the search
    clock_t start_time = clock();
    search(s2, s1);
    clock_t end_time = clock();

    // Calculate the elapsed time in milliseconds
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000.0;
    printf("Number of Occurrences: %d\n", count_fa);
    printf("Time taken for search: %.3f milliseconds\n", time_taken);

    free(s1);
    free(v1);
    return 0;
}














