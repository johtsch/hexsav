#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#define STD_PATH "rawdata.hex"

void usage(char *prog){
    printf("Usage: Usage: %s [-f file] <hexstream>\n", prog);
    exit(0);
}

void fatal(char *msg);              // handle fatal errors
void *ec_malloc(unsigned int size); // error checked malloc()

/* return 0 is true -1 is false */
int ishexstream(char *hexstr);
int writehexstr(int fd, char *hexstr);  

int main(int argc, char **argv){
    int fd;             // filedescriptor
    char *fpath;        // filepath
    char *hexstr;

    //printf("%d, %s\n", argc, argv[1]);

    if(argc == 1)
        usage(argv[0]);
    else if(strcmp(argv[1], "-f") == 0 && argc < 4)
        usage(argv[0]);
    else if(strcmp(argv[1], "-f") != 0 && argc >= 3)
        usage(argv[0]);
    

    // if alternate filepath is specified load this path
    if(strcmp(argv[1], "-f") == 0){
        // allocate memory
        fpath = ec_malloc(strlen(argv[3]));
        strcpy(fpath, argv[2]);
        hexstr = argv[3];
    }   
    // or simply use the standard path
    else{
        // allocate memory
        fpath = ec_malloc(strlen(STD_PATH));
        strcpy(fpath, STD_PATH);
        hexstr = argv[1];
    }

    //check if specified hexstream is a real hexstream

    if(ishexstream(hexstr) == -1){
        printf("[Fail] Input stream %s isn't a hexstream.\n", hexstr);
        free(fpath);
        exit(EXIT_FAILURE);
    }

    // open file
    fd = open(fpath, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);

    if(fd == -1){
        free(fpath);
        fatal("in main() opening file");
    }

    if(writehexstr(fd, hexstr) == -1){
        free(fpath);
        fatal("in main() writehexstr() failed");
    }

    if(close(fd) == -1){
        free(fpath);
        fatal("in main() closing failed");
    }

    return 0;
}


void fatal(char *msg){
    char err_msg[100];
    strcpy(err_msg, "[!!] Fatal Error ");
    strncat(err_msg, msg, 83);
    perror(err_msg);
    exit(-1);
}

void *ec_malloc(unsigned int size){
    void *ptr = malloc(size);

    if(ptr == NULL)
        fatal("in ec_malloc() allocating memory");

    return ptr;
}

int ishexstream(char *hexstr){
    if(strlen(hexstr) % 2 != 0){
        return -1;
    }

    for(int i = 0; i < strlen(hexstr); ++i){
        if(!(hexstr[i] > 47 && hexstr[i] < 58 || hexstr[i] > 96 && hexstr[i] < 103))
            return -1;
    }

    return 0;
}


int writehexstr(int fd, char *hexstr){
    char tmp[3];
    tmp[2] = '\0';
    for(int i = 0; i < strlen(hexstr); i+=2){
        tmp[0] = hexstr[i];
        tmp[1] = hexstr[i+1];
        char byte = (char)strtol(tmp, NULL, 16);

        if(write(fd, (void*)&byte, 1) == -1)
            return -1;  
    }
    return 0;
}