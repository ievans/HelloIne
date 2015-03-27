#include <stdio.h>
#define MAX_COLORS  256

// example from stack overflow

int * global;

typedef struct {
    char* name;
    int red;
    int green;
    int blue;
} Color;

extern void printColor(Color * c);

Color Colors[MAX_COLORS];

void eachColor (void (*fp)(Color *c)) {
    int i;
    for (i=0; i<MAX_COLORS; i++)
        (*fp)(&Colors[i]);
}

int evilfunction() {
    printf("i am evil");
    return 0xdeadbeef;
}

void printColor(Color* c) {
    if (c->name)
        printf("%s = %i,%i,%i\n", c->name, c->red, c->green, c->blue);
}

int main() {
    printf("hello world\n");
    int i =0;
    for (i = 0; i < 10; i++) {
        i++;
    }
    if (i > 10) {
        printf("A\n");
    } else {
        printf("B\n");
    }
    Colors[0].name="red";
    Colors[0].red=255;
    Colors[1].name="blue";
    Colors[1].blue=255;
    Colors[2].name="black";

    printf("address of print color is %p\n", printColor);
    eachColor(printColor);

    // we want to store print color without invoking a function
    global = (int *)printColor;


    // we want to declare a pointer to the function but not actually assign it
    int (*myfunction)(char * buff);

    return 0;
}
