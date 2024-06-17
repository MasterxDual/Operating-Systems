/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 8, Ejercicio 1
 Objetivo: Comprender paginación
 Fecha: 29/05/2024
\**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FRAMES 10
#define REFERENCE_STRING_LENGTH 30

//Defining the enumerator to select the algorithm
typedef enum {FIFO, LRU, OPT} Algorithm;

void read_input(const char *filename, int *frames, char *reference_string);
int simulate_fifo(int frames, const char *reference_string);
int simulate_lru(int frames, const char *reference_string);
int simulate_opt(int frames, const char *reference_string);
void print_results(int frames, const char *reference_string, int faults, Algorithm algo);
void save_results(const char *filename, const char *algo_name, int frames, int faults);

int main(int argc, char const *argv[]) {
    Algorithm algo;

    //Verifying Command Line Arguments
    if(argc != 4) {
        fprintf(stderr, "Usage: %s -a <algorithm> input_file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //Determination of the selected algorithm
    if(strcmp(argv[2], "FIFO") == 0) {
        algo = FIFO;
    } else if (strcmp(argv[2], "LRU") == 0) {
        algo = LRU;
    } else if (strcmp(argv[2], "OPT") == 0) {
        algo = OPT;
    } else {
        fprintf(stderr, "Invalid algorithm. Use FIFO, LRU, or OPT.\n");
        exit(EXIT_FAILURE);
    }

    int frames;
    char reference_string[REFERENCE_STRING_LENGTH + 1];

    //Reading input file
    read_input(argv[3], &frames, reference_string);

    int faults;
    //Simulation execution according to the selected algorithm
    switch (algo) {
        case FIFO:
            faults = simulate_fifo(frames, reference_string);
            break;
        case LRU:
            faults = simulate_lru(frames, reference_string);
            break;
        case OPT:
            faults = simulate_opt(frames, reference_string);
            break;
    }

    //Printing and saving results
    print_results(frames, reference_string, faults, algo);
    save_results("results.csv", argv[2], frames, faults);

    return 0;
}

//Function to read the input file
void read_input(const char *filename, int *frames, char *reference_string) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    fscanf(file, "%d\n%s", frames, reference_string);
    fclose(file);
}

//Simulation of FIFO algorithm
int simulate_fifo(int frames, const char *reference_string) {
    int faults = 0;
    int frame[MAX_FRAMES];
    int next_replace = 0;

    //Initializing page frames
    for(int i = 0; i < frames; i++) {
        frame[i] = -1;
    }

    //Simulation of access to pages
    for(int i = 0; i < REFERENCE_STRING_LENGTH; i++) {
        int page = reference_string[i] - '0';
        int found = 0;
        //Checking if the page is already in a frame
        for (int j = 0; j < frames; j++) {
            if (frame[j] == page) {
                found = 1;
                break;
            }
        }
        //If the page is not found, a page fault occurs
        if(!found) {
            frame[next_replace] = page;
            next_replace = (next_replace + 1) % frames;
            faults++;
        }
    }
    return faults;
}

int simulate_lru(int frames, const char *reference_string) {
    int faults = 0;
    int frame[MAX_FRAMES];
    int time[MAX_FRAMES];
    int clock = 0;

    //Initialization of page frames and timings
    for (int i = 0; i < frames; i++) {
        frame[i] = -1;
        time[i] = 0;
    }

     //Simulation of access to pages
    for (int i = 0; i < REFERENCE_STRING_LENGTH; i++) {
        int page = reference_string[i] - '0';
        int found = 0;
        int lru_index = 0;

        //Checking if the page is already in a frame
        for (int j = 0; j < frames; j++) {
            if (frame[j] == page) {
                found = 1;
                time[j] = clock++;
                break;
            }
            //Find the index of the least recently used page
            if (time[j] < time[lru_index]) {
                lru_index = j;
            }
        }

        //If the page is not found, a page fault occurs
        if (!found) {
            frame[lru_index] = page;
            time[lru_index] = clock++;
            faults++;
        }
    }
    return faults;
}

int simulate_opt(int frames, const char *reference_string) {
    int faults = 0;
    int frame[MAX_FRAMES];

    //Initializing page frames
    for(int i = 0; i < frames; i++) {
        frame[i] = -1;
    }

    //Simulation of access to pages
    for(int i = 0; i < REFERENCE_STRING_LENGTH; i++) {
        int page = reference_string[i] - '0';
        int found = 0;

        //Checking if the page is already in a frame
        for(int j = 0; j < frames; j++) {
            if (frame[j] == page) {
                found = 1;
                break;
            }
        }

        //If the page is not found, a page fault occurs
        if(!found) {
            int replace_index = -1;
            int farthest = i + 1;

            //Determine which frame to replace by looking forward in the reference chain
            for(int j = 0; j < frames; j++) {
                int k;
                for(k = i + 1; k < REFERENCE_STRING_LENGTH; k++) {
                    if(frame[j] == reference_string[k] - '0') {
                        if(k > farthest) {
                            farthest = k;
                            replace_index = j;
                        }
                        break;
                    }
                }
                if(k == REFERENCE_STRING_LENGTH) {
                    replace_index = j;
                    break;
                }
            }

            //If it has not been determined which frame to replace, the first empty frame is used
            if(replace_index == -1) {
                for(int j = 0; j < frames; j++) {
                    if(frame[j] == -1) {
                        replace_index = j;
                        break;
                    }
                }
            }

            //Page replacement
            frame[replace_index] = page;
            faults++;
        }
    }
    return faults;
}

//Function to print the results
void print_results(int frames, const char *reference_string, int faults, Algorithm algo) {
    const char *algo_names[] = { "FIFO", "LRU", "OPT" };
    printf("Algorithm: %s\n", algo_names[algo]);
    printf("Frames: %d\n", frames);
    printf("Reference String: %s\n", reference_string);
    printf("Page Faults: %d\n", faults);
    printf("\n");
}

//Function to save results to CSV file
void save_results(const char *filename, const char *algo_name, int frames, int faults) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%s,%d,%d\n", algo_name, frames, faults);
    fclose(file);
}