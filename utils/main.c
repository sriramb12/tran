#include "conf.h"
int main()
{
   int i=0;
   char* v;
   for (i=0; confParams[i].key[0]; i++)
   {
       printf("%s %s\n", confParams[i].key, confParams[i].value);
   }
   v = getValue("vRUNDIR");
   if(v)
   printf("%s\n", v);
}
