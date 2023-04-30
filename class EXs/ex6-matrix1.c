#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#define MATRIX_DIM 3

void error (char *msg){
    //print error and exit
    perror(msg);
    exit(1);
}

typedef struct{
   int mat1[MATRIX_DIM][MATRIX_DIM];
   int mat2[MATRIX_DIM][MATRIX_DIM];
   int res [MATRIX_DIM][MATRIX_DIM];
} data;

int main(){
    int fd_shm;
    data *shm_ptr;
    //get shared memory
    if((fd_shm = shm_open("/shm", O_RDWR | O_CREAT, 0660)) == -1)
        error("shm_open");
    if(ftruncate(fd_shm, sizeof(data)) == -1)
        error("ftruncate");
    if((shm_ptr = mmap(NULL, sizeof(data), PROT_WRITE, MAP_SHARED, fd_shm, 0)) == MAP_FAILED)
        error("mmap");

    for(int i = 0 ; i < MATRIX_DIM ; i++){   
        for(int j = 0 ; j < MATRIX_DIM ; j++){
            scanf("%d", &shm_ptr->mat1[i][j]);
        }
    }

    for(int i = 0 ; i < MATRIX_DIM ; i++){   
        for(int j = 0 ; j < MATRIX_DIM ; j++){
            scanf("%d", &shm_ptr->mat2[i][j]);
        }
    }   

    munmap(shm_ptr, sizeof(data));
    return 0;
}