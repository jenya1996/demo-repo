#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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

int rowCallc (data *d, int i, int j){
    int res = 0;
    for(int k = 0; k < MATRIX_DIM; k++){
        res = res + d->mat1[i][k]*d->mat2[k][j];
    }
    return res;
}
int main(){
    int fd_shm;
    data *shm_ptr;
    //get shared memory
    if((fd_shm = shm_open("/shm", O_RDWR, 0)) == -1)
        error("shm_open");
    if((shm_ptr = mmap(NULL, sizeof(data), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm,0)) == MAP_FAILED)
        error("mmap");
    
    // for(int i = 0 ; i < MATRIX_DIM ; i++){   
    //     for(int j = 0 ; j < MATRIX_DIM ; j++){
    //         printf("%d,", shm_ptr->mat1[i][j]);
    //     }
    //     printf("\n");
    // } 
    // printf("\n");  
    // for(int i = 0 ; i < MATRIX_DIM ; i++){   
    //     for(int j = 0 ; j < MATRIX_DIM ; j++){
    //         printf("%d,", shm_ptr->mat2[i][j]);
    //     }
    //     printf("\n");
    // }
    pid_t p;
    for (int i = 0; i < MATRIX_DIM; i++){
        p = fork();
        if(p == 0){
            //matrix callculation for row i
            for (int j = 0; j < MATRIX_DIM; j++){
                shm_ptr->res[i][j] = rowCallc(shm_ptr, i,j);
            }
        return 0;
        }
    }
    if (p != 0){
        wait(NULL);
        for(int i = 0 ; i < MATRIX_DIM ; i++){   
            for(int j = 0 ; j < MATRIX_DIM ; j++){
                printf("%d,", shm_ptr->res[i][j]);
            }
            printf("\n");
        }
        munmap(shm_ptr, sizeof(data));
        shm_unlink("/shm");
    }
    return 0;
}