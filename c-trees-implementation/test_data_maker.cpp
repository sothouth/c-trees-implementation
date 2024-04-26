#include<stdio.h>
#include<stdlib.h>
#include<time.h>

char str[30];
char* rr(int n) {
    for (int i = 0; i < n; ++i)
        str[i] = rand() % 26 + 'a';
    str[n] = 0;
    return str;
}
int main() {
    srand(time(0));
    register int n, nown;
    FILE* F = fopen("data.dat", "w");
    scanf("%d", &n);
    for (int i = 0; i < n; ++i) {
        printf("%d\n", i);
        nown = rand() % 5 + 1;
        fprintf(F, "%s", rr(rand() % 24 + 5));//30
        for (int j = 0; j < nown; ++j)
            fprintf(F, " %s", rr(rand() % 17 + 3));//20
        fprintf(F, " E\n");
    }
    fclose(F);
    return 0;
}
