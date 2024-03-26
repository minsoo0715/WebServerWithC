#include <stdio.h>
#include <stdlib.h>

#include "common/error.h"

void error(char *msg)
{
   perror(msg);
   exit(1);
}