struct param
{
  char key[20];
  char value[20];
};

struct param confParams [] = {
 {"RUNDIR", "./run"}, 
 {"DATADIR", "data"}, 
 {"LOGDIR", "log"}, 
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
