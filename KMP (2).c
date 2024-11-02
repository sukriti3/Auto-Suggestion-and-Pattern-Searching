#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // For measuring execution time

// Dynamic array to store positions of line breaks in the text
long long *v1 = NULL;
int v1_size = 0;      // Number of lines in the file
int count_kmp = 0;    // Counter for pattern occurrences

// Function to compute the Longest Prefix Suffix (LPS) array for the pattern
void computeLPSArray(char *pat, int M, int *lps) {
    int len = 0;       // Length of the previous longest prefix suffix
    lps[0] = 0;        // LPS for the first character is always 0

    int i = 1;
    while (i < M) {
        if (pat[i] == pat[len]) { // Characters match
            len++;
            lps[i] = len;
            i++;
        } else {                  // Mismatch after len matches
            if (len != 0) {
                len = lps[len - 1];  // Backtrack to the last valid prefix suffix length
            } else {
                lps[i] = 0;          // No valid prefix suffix found
                i++;
            }
        }
    }
}

// Function to search for occurrences of the pattern in the text using KMP algorithm
void KMPSearch(char *pat, char *txt) {
    int M = strlen(pat);  // Length of the pattern
    int N = strlen(txt);  // Length of the text

    // Allocate memory for LPS array
    int *lps = (int *)malloc(M * sizeof(int));
    if (!lps) {
        printf("Memory allocation failed.\n");
        return;
    }

    // Preprocess the pattern to fill the LPS array
    computeLPSArray(pat, M, lps);

    int i = 0; // Index for txt
    int j = 0; // Index for pat

    while ((N - i) >= (M - j)) { // Continue until text has remaining characters to match
        if (pat[j] == txt[i]) { // Characters match
            j++;
            i++;
        }

        if (j == M) { // Pattern found
            // Calculate line number and position within that line
            long long it = 0;
            while (it < v1_size && v1[it] <= i - j)
                it++;

            int c;
            if (it == 0) {
                c = i - j;
            } else {
                c = i - j - v1[it - 1];
            }
            if (it == 0) {
                c++;
            }

            // Identify the start and end of the word containing the matched pattern
            int start = i - j;
            while (start > 0 && txt[start - 1] != ' ')
                start--;

            int end = i;
            while (end < N && txt[end] != ' ')
                end++;

            // Extract the matched word
            int word_len = end - start;
            char *word = (char *)malloc(word_len + 1);
            if (!word) {
                printf("Memory allocation failed.\n");
                free(lps);
                return;
            }
            strncpy(word, txt + start, word_len);
            word[word_len] = '\0';

            // Print the result
            count_kmp++;
            printf("Found '%s' at line: %lld position: %d\n", word, it + 1, c);

            free(word);
            j = lps[j - 1]; // Move to the next possible match using LPS array
        } else if (i < N && pat[j] != txt[i]) { // Mismatch after j matches
            if (j != 0) {
                j = lps[j - 1]; // Use LPS array to skip unnecessary comparisons
            } else {
                i++; // Move to the next character in text
            }
        }
    }

    free(lps); // Free allocated memory for LPS array
}

int main() {
    FILE *newfile = fopen("sherlock.txt", "r");
    if (!newfile) {
        printf("Failed to open the file.\n");
        return 1;
    }

    char *s1 = NULL;           // Complete text of the file
    size_t s1_len = 0;         // Length of the complete text
    size_t buffer_size = 1024; // Buffer size for reading lines
    char *buffer = (char *)malloc(buffer_size); // Buffer to hold each line

    if (!buffer) {
        printf("Memory allocation failed.\n");
        fclose(newfile);
        return 1;
    }

    // Read the file line by line and accumulate the content in `s1`
    while (fgets(buffer, buffer_size, newfile)) {
        size_t line_len = strlen(buffer);

        // Resize `s1` to fit the new line, a space, and null terminator
        s1 = realloc(s1, s1_len + line_len + 2); // +2 for space and null terminator
        if (!s1) {
            printf("Memory allocation failed.\n");
            free(buffer);
            fclose(newfile);
            return 1;
        }

        // Append the line to `s1`
        memcpy(s1 + s1_len, buffer, line_len);
        s1_len += line_len;
        s1[s1_len] = ' ';
        s1_len++;
        s1[s1_len] = '\0';

        // Store cumulative length of each line in `v1` for line break positions
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

    // Read the pattern to search for
    char s2[256];
    printf("Enter pattern to search: ");
    fgets(s2, sizeof(s2), stdin);
    s2[strcspn(s2, "\n")] = '\0'; // Remove newline character

    // Measure the execution time for searching the pattern
    clock_t start = clock();
    KMPSearch(s2, s1);
    clock_t end = clock();

    // Print the total number of occurrences and execution time
    printf("Number of Occurrences: %d\n", count_kmp);
    double time_taken = ((double)(end - start) / CLOCKS_PER_SEC) * 1000; // Time in milliseconds
    printf("Execution time: %.2f ms\n", time_taken);

    // Free allocated memory
    free(s1);
    free(v1);

    return 0;
}















