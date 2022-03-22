/* File : example.c */
 
#include <time.h>
#include <stdio.h>
 double My_variable = 3.0;
 
 int fact(int n) {
     if (n <= 1) return 1;
     else return n*fact(n-1);
 }
 
 int my_mod(int x, int y) {
     return (x%y);
 }
 	
 char *get_time()
 {
     time_t ltime;
     time(&ltime);
     return ctime(&ltime);
 }
 
 typedef const char * TRANS_FILE_TYPE;
 
char global_ptr[30] = "test\n"; 

void 
 icom_put_compress_init(int delete, char *msg_ptr, char *out)
 {
      static const char *mens = "test\n";
      
      printf("\n %p %s  mens_addr = %p\n", global_ptr, global_ptr, mens);
      
     
      *out = &global_ptr;
 }
  
  

  
char * printmsg()
{
      printf("getmsg()=%s\n", global_ptr);
      return global_ptr;
}
 