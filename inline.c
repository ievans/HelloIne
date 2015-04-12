int main() {
    char * str = "hello there";
    int len = 20;
    int a=10, b;
    __asm__ volatile ("movl %1, %%eax; movl %%eax, %0;"
         :"=r"(b)        /* output */
         :"r"(a)         /* input */
         :"%eax"         /* clobbered register */
        );

}
