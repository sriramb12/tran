#ifndef opclib_client_msg_h
#define opclib_client_msg_h


#define OPTIMIZE_BANNER \
"\nWelcome To version 0.2 of Optimize\n\
(OPC Template Maker)\n\n"

#define OPTIMIZE_SUMMARY \
"\n\nMain menu:  l, p, pl, m, s, q, e\n\n"
#define OPTIMIZE_HELP \
"\n\nMain Menu\n\n\
l:    OPC template library commands\n\
p:    Process commands\n\
pl:   Process and layer commands\n\
m:    Display this menu\n\
s:    Display menu summary\n\
q:    Quit Optimize\n\
e:    Exit Optimize\n\n"

#define LIBRARY_SUMMARY \
"\n\nMain Menu/Library Commands:  V, v, c, d, p, S, m, s, S, g, G, t, h, q, e\n\n"

#define LIBRARY_MSG \
"\n\nOPC Template Library Commands\n\n\
V:  View list of OPC template libraries\n\
v:  View processes in an OPC template library <library>\n\
c:  Create new OPC template library <library name> <PIN twice>\n\
d:  Delete OPC template library <library> <PIN>\n\
p:  change PIN for OPC template library <library> <old PIN> <new PIN twice>\n\
S:  Set OPC template library for following transactions <library> <PIN>\n\
h:  Help on specified command\n\
m:  Display this menu\n\
s:  Display menu summary\n\
S:  Show current OPC template library status\n\
g:  Get heirarchial list of current OPC template library\n\
G:  Get verbose listing of current OPC template library\n\
t:  Display current template library, process, and layer\n\
q:  Quit current screen and return to main menu\n\
e:  Exit Optimize\n\n"

#define PROCESS_SUMMARY \
"\n\nMain Menu/Process Commands:\n\n\
    v,V,A,c,n,f,r,R,l,L,o,O,x,X,i,I,D,d,p,P,h,H,m,s,S,g,G,t,q,e\n\n"

#define PROCESS_MSG \
"\n\nProcess Commands\n\n\
The following commands access data for a specified library and process.\n\
The library was initialized when you brought up Optimize \n\
and can be changed with the 'Set Library' command.\n\
File names may be specified using wild cards.\n\
The 'R', 'L', 'O', 'X', 'I', and 'D' options operate on all\n\
files in the current process and layer.\n\n\
V:  View list of layers in a process <process>\n\
v:  View list of files in a process <process>\n\
A:  Retrieve list of layers associated with a file <process> <file>\n\
c:  Create new process <process>\n\
n:  Clone an existing process <new process> <old process>\n\
f:  Retrieve copy of all form templates in a process <process>\n\
r:  Read copy of files in a process <process>\n\
l:  Lock for edit files in a process <process>\n\
o:  Checkout for edit files in a process <process>\n\
x:  Cancel check out on files in a process <process> [<files>]\n\
i:  Check in files in a process <process> [<files>]\n\
D:  Delete process and all layers and files in the process <process>\n\
d:  Delete files from process <process> <files>\n\
p:  Read prior version of file in process <process> <version> <file>\n\
P:  Check out prior version of file in process <process> <version> <file>\n\
H:  Retrieve version history on file <process> <file>\n\
h:  Help on specified command\n\
m:  Display this menu\n\
s:  Display menu summary\n\
S:  Show current OPC template library status\n\
g:  Get heirarchial list of current OPC template library\n\
G:  Get verbose listing of current OPC template library\n\
t:  Display current template library, process, and layer\n\
q:  Quit current screen and return to main menu\n\
e:  Exit Optimize\n\n"

#define PROCESS_LAYER_SUMMARY \
"\n\nMain Menu/Process/Layer Commands:\n\n\
    V,V,A,c,n,r,R,l,L,o,O,x,X,i,I,a,D,d,p,P,h,H,m,s,S,g,G,t,q,e\n\n"

#define PROCESS_LAYER_MSG \
"\n\nProcess And Layer Commands\n\n\
The following commands access data for a specified process and layer.\n\
The library was initialized when you brought up Optimize \n\
and can be changed with the 'Set Library' command.\n\n\
File names may be specified using wild cards.\n\n\
The 'R', 'L', 'O', 'X', 'I', and 'D' options operate on all\n\
files in the current process and layer.\n\n\
V:  View list of layers in a process <process>\n\
v:  View list of files in process and layer <process> <layer>\n\
A:  Retrieve list of layers associated with a file <process> <file>\n\
c:  Create new layer in an existing process <process> <layer>\n\
n:  Clone an existing layer <process> <new layer> <process <old layer>\n\
r:  Read copy of files in process and layer <process> <layer> [<files>]\n\
l:  Lock for edit files in process and layer <process> <layer> [<files>]\n\
o:  Check out for edit files in process and layer <process> <layer> [<files>]\n\
x:  Cancel checkout on files in process and layer <process> <layer>  [<files>]\n\
i:  Check in files for process and layer <process> <layer>  [<files>]\n\
a:  Associate files with process and layer <process> <layer>  [<files>]\n\
d:  Delete files in process and layer <process> <layer>  [<files>]\n\
p:  Read prior version of file in process <process> <version> <file>\n\
P:  Check out prior version of file in process <process> <version> <file>\n\
H:  Retrieve version history on file <process> <file>\n\
h:  Help on specified command\n\
m:  Display this menu\n\
s:  Display menu summary\n\
S:  View current OPC template library status\n\
g:  Get heirarchial list of current OPC template library\n\
G:  Get verbose listing of current OPC template library\n\
t:  Display current template library, process, and layer\n\
q:  Quit current screen and return to main menu\n\
e:  Exit Optimize\n\n"

#endif /* Define for opclib_client_msg_h */
