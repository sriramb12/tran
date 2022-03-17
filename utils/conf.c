#include "conf.h"
struct param confParams [] = {
 {EXT_SERVER, "RUNDIR", "./run"},
 {EXT_SERVER, "DATADIR", "data"},
 {EXT_SERVER, "LOGDIR", "log"},
};
char* getValue(char* key)
{
   int i; 
   for (i=0; confParams[i].key[0]; i++)
   {
       if(!strcmp(key, confParams[i].key))
               return confParams[i].value;
   }
   return 0; 
}
/*
 * Driver/Test Function
 */
#ifdef TEST
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
#endif // TEST
