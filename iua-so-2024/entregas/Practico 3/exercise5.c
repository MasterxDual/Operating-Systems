/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 3, Ejercicio 5
 Objetivo: Aprendiendo a utilizar la funcion stat().
 Fecha: 15/04/2024
\**************************************************************/

#define _DEFAULT_SOURCE //Now I can use MACROS S_IMFT, S_IFBLK, and so on.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/sysmacros.h>
#include <stdint.h>
#include <errno.h>
#include <sys/stat.h>
#include <linux/fs.h>

//Variable with error number if it occurs.
extern int errno;

void printFileProperties(struct stat stats);

int main(int argc, char *argv[]) {
    /*argv[0] has the name of the program executed in shell: ./my_program
    For example: if we write the following command in shell: ./my_program hello there, array argv will be:
    argv[0] = ./my_program
    argv[1] = hello
    argv[2] = there
    We need to use argv[1] to receive the name of the file to be read. So WE CAN USE ANY WORD OF THE NEW FILE TO BE CREATED AND WRITTEN.
    */
    char *data_path = argv[1];
    struct stat stats; //Struct buff to be used to save files read from file

    if(stat(data_path, &stats) == 0) {
        printFileProperties(stats);
    } else {
        printf("Error number %d\n", errno);
        perror("Unable to get file properties.");
        exit(EXIT_FAILURE);
    }

    return 0;
}

/*st_ino: The file ID for the object. This number uniquely identifies the object within a file system. When st_ino and st_dev 
    are used together, they uniquely identify the object on the system.
st_nlink: The number of links to the file. This field will be 65,535 if the value could not fit in the specified nlink_t
    field. The complete value will be in the st_nlink32 field.
st_uid: The numeric user ID (uid) of the owner of the file.
st_gid: The numeric group ID (gid) for the file.
st_size: Defined as follows for each file type:
    Regular File
        The number of data bytes in the file.
    Directory
        The number of bytes allocated to the directory.
    Symbolic Link
        The number of bytes in the path name stored in the symbolic link.
    Local Socket
        Always zero.
    Operating System Native Object
        This value is dependent on the object type. 
st_blksize: The block size of the file in bytes. This number is the number of bytes in a block of disk unit storage. 
st_blocks: Number of 512B blocks allocated
*/

/*MACROS usage:

S_IFBLK(mode)
    Is nonzero for block special files
S_IFCHR(mode)
    Is nonzero for character special files
S_IFDIR(mode)
    Is nonzero for directories
S_IFFIFO(mode)
    Is nonzero for pipes and FIFO special files
S_IFREG(mode)
    Is nonzero for regular files
S_IFLNK(mode)
    Is nonzero for symbolic links
S_IFSOCK(mode)
    Is nonzero for local sockets
S_IFNATIVE(mode) (this MACRO didn't work for me)
    Is nonzero for operating system native objects 
*/

void printFileProperties(struct stat stats) {
    printf("File properties:\n");
    printf("I-node number: %ju\nLink count: %ju\nUser ID of owner: %ju\nGroup ID of owner: %ju\nFile size, in bytes: %jd\n", (__uintmax_t) stats.st_ino, (__uintmax_t) stats.st_nlink, (__uintmax_t) stats.st_uid, (__uintmax_t) stats.st_gid, (__intmax_t) stats.st_size);
    printf("Preferred I/O block size: %jd\nBlocks allocated: %jd\n", (__intmax_t) stats.st_blksize, (__intmax_t) stats.st_blocks);
    printf("Last status change: %s", ctime(&stats.st_ctime));
    printf("Last file access: %s", ctime(&stats.st_atime));
    printf("Last file modification: %s", ctime(&stats.st_mtime));
    printf("File permissions (octal): %#o", stats.st_mode & ~(S_IFMT));

    printf("\nFile type:\n");
    switch (stats.st_mode & S_IFMT) {
        case S_IFBLK:
            printf("Block device\n");
            break;
        case S_IFCHR:
            printf("Character device\n");        
            break;
        case S_IFDIR:
            printf("Directory\n");               
            break;
        case S_IFIFO:
            printf("FIFO/pipe\n");               
            break;
        case S_IFLNK:
            printf("Symbolic link\n");                 
            break;
        case S_IFREG:
            printf("Regular file\n");            
            break;
        case S_IFSOCK:
            printf("Local socket\n");                  
            break;
        default:
            printf("Unknown?\n");                
            break;
    }

    /*The st_dev field provides information about the device where the file resides. Specifically:
        The major number identifies the device driver.
        The minor number encodes the physical disk (or partition).
    */
    printf("Device driver (disk): %ju\nPhysical disk (partition): %ju\n", (uintmax_t) major(stats.st_dev), (uintmax_t) minor(stats.st_dev));
    
}