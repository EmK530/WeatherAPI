#include "jansson.h"

int main() {
  json_t *root = json_object();
  if (!root) {
    fprintf(stderr, "Error creating JSON object\n");
    return 1;
  }

  return 0;
}