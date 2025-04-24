#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int global_init = 10;
int global_uninit;
const int global_const = 20;

void f_a() {
    int local_var = 30;
    static int static_var = 40;
    const int const_var = 50;
    char* ptr = "hello";
    char str[] = "hello";

    printf("Address of local_var: %p\n", &local_var);
    printf("Address of static_var: %p\n", &static_var);
    printf("Address of const_var: %p\n", &const_var);
    printf("Address of string literal 'hello': %p\n", ptr);
    printf("Address of string array 'hello': %p\n", str);
}

int* f_d() {
    int local_var = 30;
    return &local_var;
}

void f_h() {
    char* env_val = getenv("MY_VAR");
    printf("Initial MY_VAR value: %s\n", env_val ? env_val : "not set");
    setenv("MY_VAR", "new_value", 1);
    printf("Updated MY_VAR value: %s\n", getenv("MY_VAR"));
}

void f_e() {
    char* buf = malloc(100);
    strcpy(buf, "hello world from first buffer");
    printf("Buffer: %s\n", buf);
    free(buf);
    printf("Buffer after free: %s\n", buf);

    char* buf2 = malloc(100);
    strcpy(buf2, "HELLO WORLD FROM SECOND BUFFER");
    printf("Second buffer: %s\n", buf2);
    buf2 += 50;
    //free(buf2);
    printf("Middle of second buffer after incorrect free: %s\n", buf2);
    buf2 -= 50;
    printf("Second buffer after incorrect free: %s\n", buf2);
    free(buf2);
}

int main() {
    printf("PID: %d\n", getpid());

    // 1a
    printf("Address of global_init: %p\n", &global_init);
    printf("Address of global_uninit: %p\n", &global_uninit);
    printf("Address of global_const: %p\n", &global_const);
    f_a();

    printf("check /proc/%d/maps\n", getpid());
    sleep(20);

    // 1d
    int* ptr = f_d();
    printf("Returned local address: %p\n", ptr);

    // 1e:
    f_e();

    // 1h
    f_h();

    return 0;
}
