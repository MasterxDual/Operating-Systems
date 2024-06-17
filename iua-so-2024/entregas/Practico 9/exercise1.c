#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEADER_LENGTH 24
#define DIRECTORY_ENTRY_LENGTH 12
#define FIELD_TERMINATOR '\x1E'
#define RECORD_TERMINATOR '\x1D'

// Function prototypes
void read_leader(FILE *file, char *leader);
void read_directory(FILE *file, int num_fields, char **directory);
void read_fields(FILE *file, char *record_buffer, int record_length);
void parse_field(char *field, char *tag, int *length, int *start);
void print_field_data(char *record_buffer, int start, int length);

int main() {
    //Open the ISO2709 file for reading in binary mode
    FILE *file = fopen("osbooks.iso2709", "rb");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Loop to process each record in the file
    while (!feof(file)) {
        char leader[LEADER_LENGTH + 1];

        // Read the leader of the current record
        read_leader(file, leader);
        
        // Extract the length of the current record from the leader
        int record_length = atoi(leader);

        // Extract the base address of data from the leader
        int base_address_of_data = atoi(leader + 12);

        // Calculate the length of the directory
        int directory_length = base_address_of_data - LEADER_LENGTH - 1;

        // Calculate the number of fields from the directory length
        int num_fields = directory_length / DIRECTORY_ENTRY_LENGTH;
        
        // Allocate memory for the directory
        char **directory = (char **)malloc(num_fields * sizeof(char *));

        if (!directory) {
            perror("Memory allocation for directory failed");
            fclose(file);
            return EXIT_FAILURE;
        }

        // Read the directory entries
        read_directory(file, num_fields, directory);
        
        // Allocate memory for the record buffer to hold the actual data
        char *record_buffer = (char *)malloc(record_length - base_address_of_data);

        // Move the file pointer to the base address of data and read the fields
        fseek(file, base_address_of_data, SEEK_SET);
        read_fields(file, record_buffer, record_length - base_address_of_data);
        
        // Loop through each entry in the directory
        for (int i = 0; i < num_fields; i++) {
            char tag[4];
            int length, start;

            // Parse the directory entry to extract the tag, length, and start position
            parse_field(directory[i], tag, &length, &start);

            // Print the tag of the current field
            printf("Tag: %s\n", tag);

            // Print the data of the current field
            print_field_data(record_buffer, start, length);
        }

        // Free the memory allocated for the directory entries
        for (int i = 0; i < num_fields; i++) {
            free(directory[i]);
        }
        free(directory);

        // Free the memory allocated for the record buffer
        free(record_buffer);
        
        // Check if the next character is the record terminator
        if (fgetc(file) != RECORD_TERMINATOR) {
            break;
        }
    }

    // Close the file
    fclose(file);
    return EXIT_SUCCESS;
}

/**
 * Reads the first 24 characters of the file, which make up the header.
 * The header of a MARC21 record has a fixed length of 24 characters. It contains critical information such as the total 
 * length of the record and the base address of the data.
 * Without the header, we couldn't determine how to read the rest of the record.
*/
void read_leader(FILE *file, char *leader) {
    fread(leader, 1, LEADER_LENGTH, file);
    leader[LEADER_LENGTH] = '\0';
}

/**
 * Reads the directory, which contains entries of 12 characters each. Each entry describes a field in the record.
 * The directory contains entries that describe each field in the record. Each directory entry is 12 characters long and 
 * includes information about the field, its length, and its position in the record.
 * The directory acts as a table of contents, allowing the program to know where the data is located and how to interpret it.   
*/
void read_directory(FILE *file, int num_fields, char **directory) {
    for (int i = 0; i < num_fields; i++) {
        directory[i] = (char *)malloc(DIRECTORY_ENTRY_LENGTH + 1);

        if (!directory[i]) {
            perror("Memory allocation for directory entry failed");

            // Free already allocated entries
            for (int j = 0; j < i; j++) {
                free(directory[j]);
            }
            free(directory);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        fread(directory[i], 1, DIRECTORY_ENTRY_LENGTH, file);
        directory[i][DIRECTORY_ENTRY_LENGTH] = '\0';
    }
}

/**
 * Reads data fields based on the offset and length specified in the directory.
 * Read the actual content of the fields from the position indicated by the header to the end of the record.
 * It contains the actual data that we want to extract and interpret.
*/
void read_fields(FILE *file, char *record_buffer, int record_length) {
    fread(record_buffer, 1, record_length, file);
}

/**
 * Extracts the field label (tag), length and starting position of each field.
 * Extract the field tag, field length, and field start position from a directory entry.
 * We need this information to know what type of data each field contains, how much data it takes up, and where 
 * it starts in the record.
*/
void parse_field(char *field, char *tag, int *length, int *start) {
    char temp[6];
    strncpy(tag, field, 3);
    tag[3] = '\0';
    strncpy(temp, field + 3, 4);
    temp[4] = '\0';
    *length = atoi(temp);
    strncpy(temp, field + 7, 5);
    temp[5] = '\0';
    *start = atoi(temp);
}

/**
 * Prints the data of each field.
 * Copy and display data for a specific field from the record buffer, using the starting position and length extracted earlier.
 * To be able to view and use the data contained in each field of the record.
*/
void print_field_data(char *record_buffer, int start, int length) {
    char *field_data = (char *)malloc(length + 1);

    if (!field_data) {
        perror("Memory allocation for field data failed");
        return;
    }
    strncpy(field_data, record_buffer + start, length);
    field_data[length] = '\0';
    printf("%s\n", field_data);
    free(field_data);
}