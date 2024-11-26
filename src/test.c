#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

bool test_read_line_1() {


  FILE *fp = fopen("tests/01-simple-in.snk", "r");
  char *expected = "####################\n";
  char *actual = read_line(fp);
  fclose(fp);

  if (actual == NULL) return false;
  if (strcmp(expected, actual) != 0) {
    free(actual);
    return false;
  }
  free(actual);
  return true;
}

int main(){
  test_read_line_1();
}