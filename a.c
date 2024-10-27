#include <stdio.h>
#include <unistd.h>

int main(){
int x = 5;
int y = 40;

int solution = y / x;
int i;

for(i = 0; y>i; i+5){
    printf("Sleeping for 2 seconds...\n");
    sleep(2); // Sleep for 5 seconds
    printf("Done sleeping!\n");

    printf("%d\n",solution);
    y=y-5;

}

}
