#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct Student{
    char name[15];
    char age[5];
    char address[20];
    char info[160];
} Student;
