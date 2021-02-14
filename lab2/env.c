
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/types.h>
#include <pwd.h>
#include <stdbool.h>

extern char **environ;
int main(void)
{
int i;
for (i = 0; environ[i] != NULL; i++)
printf("%s\n", environ[i]);
exit(EXIT_SUCCESS);
}