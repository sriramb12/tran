/* example.i */
%module example
%typemap(argout) char *OUTPUT {
        if (argvi >= items) {
                EXTEND(sp,1);              /* Extend the stack by 1 object */
        }
        $result = sv_newmortal();
        sv_setiv($target,(IV) *($1));
        argvi++;
}
 %{
 /* Put header files here or function declarations like below */
 extern double My_variable;
 extern int fact(int n);
 extern int my_mod(int x, int y);
 extern char *get_time();
 extern void icom_put_compress_init(int delete, char *msg_ptr, char *OUTPUT);
 extern char *printmsg();
 %}
 
 extern double My_variable;
 extern int fact(int n);
 extern int my_mod(int x, int y);
 extern char *get_time();
 extern void icom_put_compress_init(int delete, char *msg_ptr, char *OUTPUT);
 extern char *printmsg();