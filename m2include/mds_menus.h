#define output_line(row, col, string) \
{\
    mvaddstr(row, col, string);\
    clrtoeol();\
    refresh();\
}
#define MENU_ROWS 20
#define MENU_COLS 48
#define OUTPUT_ROW 18

#define MDSADMIN_BANNER \
"\n       Welcome to version 2.0.1 of MDSadmin\n\
               (MDS database administration)\n\
      Free Scale Semiconductor Confidential Proprietary software\n\
        Not for distribution outside Free Scale\n\n"

static char main_menu[MENU_ROWS][MENU_COLS] =
{
    "MAIN MENU",
    "a  Authorization functions",
    "f  Fab templates",
    "g  Global files",
    "m  Mask sets",
    "M  Machine tables",
    "p  PC libraries",
    "q  Quit current menu",
	"x  Exit program",
    "\0",
};
static char mask_menu[MENU_ROWS][MENU_COLS] =
{
    "MASK MENU",
    "a  Add user",
    "c  Cancel check-out",
    "d  Delete",
    "g  Add usergroup",
"o  Check out (Not implemented)",
    "p  Print mask directory",
    "u  View users",
    "v  View user groups",
    "q  Quit current menu",
    "x  Exit program",
    "\0"
};

static char fab_menu[MENU_ROWS][MENU_COLS] =
{
    "FAB MENU",
    "a  Add user",
    "c  Cancel check-out",
    "d  Delete",
    "g  Add usergroup",
"o  Check-out (Not implemented)",
    "p  Print mask directory",
    "u  View users",
    "v  View user groups",
    "q  Quit current menu",
    "x  Exit program",
    "\0",
};

static char pc_menu[MENU_ROWS][MENU_COLS] =
{
    "PC LIBRARY MENU",
    "a  Add user",
    "c  Cancel check-out",
    "d  Delete",
    "g  Add usergroup",
"o  Check-out (Not implemented)",
    "p  Print mask directory",
    "u  View users",
    "v  View user groups",
    "q  Quit current menu",
    "x  Exit program",
    "\0",
};

static char machine_menu[MENU_ROWS][MENU_COLS] =
{
    "MACHINE MENU",
    "a  Add user",
    "A  Approve (Not implemented)",
    "c  Cancel check-out",
    "d  Delete",
    "g  Add usergroup",
"i  Check in (Not implemented)",
"o  Check out (Not implemented)",
    "p  Print current working  directory",
    "r  Read (Not implemented)",
    "u  View users",
    "v  View user groups",
    "q  Quit current menu",
    "x  Exit program",
    "\0",
};

static char global_menu[MENU_ROWS][MENU_COLS] =
{
    "GLOBAL MENU",
    "a  Add users",
    "A  Approve (Not implemented)",
    "c  Cancel check-out",
    "d  Delete",
    "g  Add user groups",
"i  Check in (Not implemented)",
"o  Check out (Not implemented)",
    "p  Print mask directory",
    "r  Read (Not implemented)",
    "u  View users",
    "v  View user groups",
    "q  Quit current menu",
    "x  Exit program",
    "\0",
};

/* System administrative menu */
static char auth_menu[MENU_ROWS][MENU_COLS] =
{
    "AUTHORIZATION FUNCTIONS",
    "a  Add template approver",
    "b  Add template builder",
    "c  Add PC builder",
    "p  Add PC approver",
    "u  Unregister user",
    "q  Quit current menu",
    "x  Exit program",
    "\0"
};
