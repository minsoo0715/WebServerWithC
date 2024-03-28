#include <stdio.h>
#include <stdlib.h>

#include "common/error.h"

/* Function for throwing error */
void error(char *msg)
{
   perror(msg);
   exit(1);
}