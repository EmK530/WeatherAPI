/*

This is the example robin added in canva for how to read the timestamp from a
file

*/

#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Användning: %s <filnamn>\n", argv[0]);
    return 1;
  }
  struct stat filinfo;
  if (stat(argv[1], &filinfo) == -1) {
    perror("stat");
    return 1;
  }
  // Senast ändrad tid
  printf("Fil: %s\n", argv[1]);
  printf("Senast modifierad: %s", ctime(&filinfo.st_mtime));
  // Om du vill jämföra med aktuell tid
  time_t nu = time(NULL);
  double skillnad = difftime(nu, filinfo.st_mtime);
  printf("Det var %.0f sekunder sedan filen ändrades.\n", skillnad);
  return 0;
}
