//
//  main.c
//  child
//
//  Created by Jb on 18/07/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("[child] Let's crash now :)\n");
    int a=0, *ptr=&a;
    ptr = NULL;
    printf("a = %d\n", *ptr);
    printf("Bye !\n");
    return 0;
}
