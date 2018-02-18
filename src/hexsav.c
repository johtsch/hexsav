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
    int fd;            		// filedescriptor
    char *fpath = NULL;        	// filepath
    char *hexstr = NULL;


    if(argc == 1)
        usage(argv[0]);

    // check given arguments

    for(int i = 1; i < argc; ++i){
	    if(strcmp(argv[i], "-f") == 0){
		    if(i + 1 == argc)
			    fatal("in main() while checking -f argument. No filepath given");
			    
		    // allocate memory if a path is specified
		    fpath = ec_malloc(strlen(argv[i+1]));
		    strcpy(fpath, argv[i+1]);
		    i+=1;		// skip the filepath argument else it would be evaluated twice					
		    if(i >= argc)
			    break;
	    }
	    // if no argument was found we have to assume the argument is the hexstream
	    else{
		    if(ishexstream(argv[i]) == -1){
			    fprintf(stderr, "[Fail] Input stream %s is neither a hexstream nor a valid option.\n", argv[i]);
			    exit(EXIT_FAILURE);
		    }
		    else if(hexstr != NULL){
			    fprintf(stderr, "[Fail] multiple hexstreams given... just pass one.\n");
			    exit(EXIT_FAILURE);
		    }
		    // if it could be the hexstream than take it as such
		    else{
			    hexstr = ec_malloc(strlen(argv[i]));
			    strcpy(hexstr, argv[i]);
		    }
	    }
    }

    // check that a hexstream was specified
    if(hexstr == NULL){
	    fprintf(stderr, "[Fail] no hexstream specified.\n");
	    exit(EXIT_FAILURE);
    }   

    // use the standard path if no file option was found
    if(fpath == NULL){
        // allocate memory
        fpath = ec_malloc(strlen(STD_PATH));
        strcpy(fpath, STD_PATH);
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
