#include <stdio.h>
#include <stdlib.h>


int main(){

    int *bite = malloc(sizeof(int));
    *bite = 4;

    printf("%d\n", *bite);




    return 0;


    
}