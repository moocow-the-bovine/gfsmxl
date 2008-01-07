#include <stdio.h>

int main(void) {
  int i,j, n=4;

  for (i=0; i < n; i++) {
    for (j=i+1; j < n; j++) {
      printf("i=%d, j=%d", i,j);
      if (i==0 && j==2) { printf(": goto nexti!\n"); goto nexti; } //-- skip i==0,j==2
      printf(": EVAL\n");
    }
  nexti:
    for ( ; j < n; j++) {
      printf("i=%d,j=%d: SKIP\n", i,j);
    }
  }

  return 0;
}
