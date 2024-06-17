/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 7, Ejercicio 1
 Objetivo: Comprender asignación contigua
 Fecha: 20/05/2024
\**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#define MEMORY_SIZE 64 //Memory size in MB
#define OS_MEMORY 8 //OS memory in MB
#define MAX_PARTITION_SIZE 8 //Maximum partition size in MB
#define MAX_PROCESSES 100
#define MAX_PARTITIONS 10

extern int errno;

typedef struct {
    int proc_num;
    int arrival_time;
    int size;
    int finish_time;
} Process;

typedef struct Partition {
    int size;
    bool is_free;
    struct Partition *next;
} Partition;

typedef struct {
    Partition *head;
    int total_size;
} DynamicMemory;

void read_input_file(const char *filename, Process processes[], int *num_processes);
void init_fixed_partitions_same_size(Partition partitions[], int *num_partitions, bool verbose);
void init_fixed_partitions_different_size(Partition partitions[], int *num_partitions, bool verbose);
void init_dynamic_memory(DynamicMemory *memory);
Partition *create_partition(int size);
void add_partition(DynamicMemory *memory, int size);
int first_fit(Partition partitions[], int num_partitions, int process_size);
int best_fit(Partition partitions[], int num_partitions, int process_size);
int worst_fit(Partition partitions[], int num_partitions, int process_size);
Partition *first_fit_dynamic(DynamicMemory *memory, int process_size);
Partition *best_fit_dynamic(DynamicMemory *memory, int process_size);
Partition *worst_fit_dynamic(DynamicMemory *memory, int process_size);
int calculateNumDynamicPartitions(DynamicMemory *memory);
void simulate_allocation(Process processes[], int num_processes, Partition partitions[], int num_partitions, int (*allocation_function)(Partition[], int, int), bool verbose);
void simulate_dynamic_allocation(Process processes[], int num_processes, DynamicMemory *memory, Partition *(*allocation_function)(DynamicMemory*, int), bool verbose);

int main(int argc, char const *argv[]) {
    if(argc < 5) {
        //Prints in shell console that string indicating that there happened an error
        fprintf(stderr, "Usage: %s -v -a <algorithm> input_file\n", argv[0]);
        return EXIT_FAILURE;
    }

    int num_processes;
    bool verbose = strcmp(argv[1], "-v") == 0;
    const char *algorithm = argv[3];
    const char *input_file = argv[4];

    Process processes[MAX_PROCESSES];
    read_input_file(input_file, processes, &num_processes);

    Partition partitions[MAX_PARTITIONS];
    int num_partitions;

    if(strcmp(algorithm, "PI") == 0) {
        init_fixed_partitions_same_size(partitions, &num_partitions, verbose);
        simulate_allocation(processes, num_processes, partitions, num_partitions, first_fit, verbose);
    } else if(strcmp(algorithm, "PD") == 0) {
        init_fixed_partitions_different_size(partitions, &num_partitions, verbose);
        simulate_allocation(processes, num_processes, partitions, num_partitions, first_fit, verbose);
    } else {
        DynamicMemory memory;
        init_dynamic_memory(&memory);
        if(strcmp(algorithm, "DPA") == 0) {
            simulate_dynamic_allocation(processes, num_processes, &memory, first_fit_dynamic, verbose);
        } else if(strcmp(algorithm, "DFF") == 0) {
            simulate_dynamic_allocation(processes, num_processes, &memory, best_fit_dynamic, verbose);
        } else if(strcmp(algorithm, "DWF") == 0) {
            simulate_dynamic_allocation(processes, num_processes, &memory, worst_fit_dynamic, verbose);
        } else {
            //Prints in shell console that string indicating that there happened an error
            fprintf(stderr, "Invalid algorithm specified\n");
            return EXIT_FAILURE;
        }
    } 

    return 0;
}

//We read entry file and we store it in an array of processes
void read_input_file(const char *filename, Process processes[], int *num_processes) {
    FILE *file = fopen(filename, "r");

    //Verifying if there's an error opening file
    if(file == NULL) {
        printf("Error opening file...\n");
        exit(EXIT_FAILURE);
    }

    //Reading first line of text (first number in first line of txt file)
    if(fscanf(file, "%d", num_processes) == 1) {
        printf("Number of processes: %d\n", *num_processes);
    } else {
        printf("Error number %d\n", errno);
        perror("Error reading first line text\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    for (int z = 0; z < *num_processes; z++) {
        if(fscanf(file, "%d %d %d %d", &processes[z].proc_num, &processes[z].arrival_time, &processes[z].size, &processes[z].finish_time) != 4){
            printf("Error number %d\n", errno);
            printf("Error reading process %d\n", z + 1);
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);
}

//Initializing fixed partitions of same size
void init_fixed_partitions_same_size(Partition partitions[], int *num_partitions, bool verbose) {
    //We assume partition reserved for S.O. (8 M.B.) is not included in num_partitions
    int remaining_memory = MEMORY_SIZE - OS_MEMORY;
    int partition_size = MAX_PARTITION_SIZE;
    *num_partitions = remaining_memory / partition_size;

    for (int t = 0; t < *num_partitions; t++) {
        partitions[t].size = partition_size;
        partitions[t].is_free = true;
    }

    if(verbose) {
        printf("Initialized %d fixed partitions of size %dMB\n", *num_partitions, MAX_PARTITION_SIZE);
    }
}

//Initializing fixed partitions of different size
void init_fixed_partitions_different_size(Partition partitions[], int *num_partitions, bool verbose) {
    //We assume partition reserved for S.O. (8 M.B.) is not included in num_partitions
    int predefined_sizes[] = {4, 12, 20, 8, 16};

    /*num_partitions will return 5 (length of array predefined_sizes)
    sizeof(predefined_sizes) returns 5 (length of array) * 4 bytes (size of int) 
    sizeof(predefined_sizes[0] returns 4 bytes (size of int)*/
    *num_partitions = sizeof(predefined_sizes) / sizeof(predefined_sizes[0]); 

    for(int v = 0; v < *num_partitions; v++) {
        partitions[v].size = predefined_sizes[v];
        partitions[v].is_free = true;
    }

    if(verbose) {
        printf("Initialized %d fixed partitions with different sizes\n", *num_partitions);
    }
}

//Initializing dynamic memory
void init_dynamic_memory(DynamicMemory *memory) {
    memory->head = NULL;
    memory->total_size = MEMORY_SIZE - OS_MEMORY;
}

//Creating new dynamic partition
Partition *create_partition(int size) {
    Partition *new_partition = (Partition*)malloc(sizeof(Partition));
    new_partition->size = size;
    new_partition->is_free = true;
    new_partition->next = NULL;
    return new_partition;
}

//Adding new dynamic partition to existing memory
void add_partition(DynamicMemory *memory, int size) {
    Partition *new_partition = create_partition(size);
    new_partition->is_free = false;

    if(memory->head == NULL) {
        memory->head = new_partition;
    } else {
        Partition *current = memory->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_partition;
    }
}

/*The memory manager scans along the list of segments until it finds a hole that is big enough. The hole is then broken up
into two pieces, one for the process and one for the unused memory, except in the statistically unlikely case of an exact fit.
First fit is a fast algorithm because it searches as little as possible*/
int first_fit(Partition partitions[], int num_partitions, int process_size) {
    for(int w = 0; w < num_partitions; w++) {
        if(partitions[w].is_free && partitions[w].size >= process_size) {
            return w;
        }
    }
    return -1; //No suitable partition found
}

/*Best fit searches the entire list, from beginning to end, and takes the smallest hole that is adequate. Rather than breaking up
a big hole that might be needed later, best fit tries to find a hole that is close to the actual size needed, to best match
the request and the available holes
Best fit is slower than first fit because it must search the entire list every time it is called. Somewhat surprisingly, it also 
results in more wasted memory than first fit or next fit because it tends to fill up memory with tiny, useless holes.
First fit generates larger holes on the average.*/
int best_fit(Partition partitions[], int num_partitions, int process_size) {
    int best_index = -1;
    int best_size = MEMORY_SIZE + 1;

    for (int b = 0; b < num_partitions; b++) {
        if(partitions[b].is_free && partitions[b].size >= process_size && partitions[b].size < best_size) {
            best_index = b;
            best_size = partitions[b].size;
        }
    }
    
    return best_index;
}

/*To get around the problem of breaking up nearly exact matches into a process and a tiny hole, one could think about worst fit,
that is, always take the largest available hole, so that the new hole will be big enough to be useful. Simulation has shown 
that worst fit is not a very good idea either*/
int worst_fit(Partition partitions[], int num_partitions, int process_size) {
    int worst_index = -1;
    int worst_size = 0;

    for(int q = 0; q < num_partitions; q++) {
        if(partitions[q].is_free && partitions[q].size >= process_size && partitions[q].size > worst_size) {
            worst_index = q;
            worst_size = partitions[q].size;
        }
    }
    
    return worst_index;
}

/*The memory manager scans along the list of segments until it finds a hole that is big enough. The hole is then broken up
into two pieces, one for the process and one for the unused memory, except in the statistically unlikely case of an exact fit.
First fit is a fast algorithm because it searches as little as possible*/
Partition *first_fit_dynamic(DynamicMemory *memory, int process_size) {
    Partition *current = memory->head;
    while (current != NULL) {
        if (current->is_free && current->size >= process_size) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/*Best fit searches the entire list, from beginning to end, and takes the smallest hole that is adequate. Rather than breaking up
a big hole that might be needed later, best fit tries to find a hole that is close to the actual size needed, to best match
the request and the available holes
Best fit is slower than first fit because it must search the entire list every time it is called. Somewhat surprisingly, it also 
results in more wasted memory than first fit or next fit because it tends to fill up memory with tiny, useless holes.
First fit generates larger holes on the average.*/
Partition  *best_fit_dynamic(DynamicMemory *memory, int process_size) {
    Partition *best_fit = NULL;
    int smallest_size = MEMORY_SIZE + 1;

    Partition *current = memory->head;
    while(current != NULL) {
        if(current->is_free && current->size >= process_size && current->size < smallest_size) {
            best_fit = current;
            smallest_size = current->size;
        }
        current = current->next;
    }
    return best_fit;
}

/*To get around the problem of breaking up nearly exact matches into a process and a tiny hole, one could think about worst fit,
that is, always take the largest available hole, so that the new hole will be big enough to be useful. Simulation has shown 
that worst fit is not a very good idea either*/
Partition *worst_fit_dynamic(DynamicMemory *memory, int process_size) {
    Partition *worst_fit = NULL;
    int largest_size = 0;
    Partition *current = memory->head;

    while(current != NULL) {
        if(current->is_free && current->size >= process_size && current->size > largest_size) {
            worst_fit = current;
            largest_size = current->size;
        }
        current = current->next;
    }
    return worst_fit;
}

void simulate_allocation(Process processes[], int num_processes, Partition partitions[], int num_partitions, int (*allocation_function)(Partition[], int, int), bool verbose) {
    int internal_fragmentation = 0;
    int external_fragmentation = 0;
    int allocation_failures = 0;
    int used_memory = 0;

    //Calculate internal fragmentation, used memory and allocation failures
    for(int y = 0; y < num_processes; y++) {
        int index = allocation_function(partitions, num_partitions, processes[y].size);
        
        if(index != -1) {
            partitions[index].is_free = false;
            internal_fragmentation += partitions[index].size - processes[y].size;
            used_memory += processes[y].size;
            if(verbose) {
                printf("Process %d allocated to partition %d\n", processes[y].proc_num, index);
            }
        } else {
            allocation_failures++;
            if(verbose) {
                printf("Process %d failed to allocate\n", processes[y].proc_num);
            }
        }
    }

    //Calculate external fragmentation ¿Está bien calculado?
    for(int i = 0; i < num_partitions; i++) {
        if(partitions[i].is_free) {
            external_fragmentation += partitions[i].size;
        }
    }

    printf("Internal Fragmentation: %0.2f\n", (float) internal_fragmentation / num_processes);
    printf("External Fragmentation: %0.2f\n", (float) external_fragmentation / num_partitions);
    printf("Allocation Failures: %d\n", allocation_failures);
    printf("Used Memory: %d\n", used_memory);
}

//Returns number of free dynamic partitions
int calculateNumDynamicPartitions(DynamicMemory *memory) {
    Partition *current = memory->head;
    int countNodes = 0;

    while(current != NULL) {
        if(current->is_free) {
            countNodes++;
        }
        current = current->next;
    }
    return countNodes;
}

void simulate_dynamic_allocation(Process processes[], int num_processes, DynamicMemory *memory, Partition *(*allocation_function)(DynamicMemory*, int), bool verbose) {
    int internal_fragmentation = 0;
    int external_fragmentation = 0;
    int allocation_failures = 0;
    int used_memory = 0;

    for(int i = 0; i < num_processes; i++) {
        if (verbose) {
            printf("Process %d: arrival_time=%d, size=%d, finish_time=%d\n", processes[i].proc_num, processes[i].arrival_time, processes[i].size, processes[i].finish_time);
        }

        Partition *partition = allocation_function(memory, processes[i].size);
        if(partition == NULL) {
            // No suitable partition found, allocate a new one if there's enough memory
            if(memory->total_size >= processes[i].size) {
                if (verbose) {
                    printf("Allocating new partition for process %d with size %d\n", processes[i].proc_num, processes[i].size);
                }
                add_partition(memory, processes[i].size);
                memory->total_size -= processes[i].size;
                used_memory += processes[i].size;
            } else {
                if (verbose) {
                    printf("Allocation failed for process %d\n", processes[i].proc_num);
                }
                allocation_failures++;
            }
        } else {
            partition->is_free = false;
            internal_fragmentation += partition->size - processes[i].size;
            used_memory += processes[i].size;
            if (verbose) {
                printf("Allocated process %d to partition with size %d\n", processes[i].proc_num, partition->size);
            }
        }
    }

    // Calculate external fragmentation
    Partition *current = memory->head;
    while (current != NULL) {
        if (current->is_free) {
            external_fragmentation += current->size;
        }
        current = current->next;
    }

    int free_partitions = calculateNumDynamicPartitions(memory);
    int average_external_fragmentation = free_partitions > 0 ? external_fragmentation/free_partitions : 0;

    printf("Internal Fragmentation: %0.2f\n", (float) internal_fragmentation / num_processes);
    printf("External Fragmentation: %0.2f\n", (float) average_external_fragmentation);
    printf("Allocation Failures: %d\n", allocation_failures);
    printf("Used Memory: %d\n", used_memory);
}
