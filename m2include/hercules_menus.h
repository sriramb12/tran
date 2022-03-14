#define output_line(row, col, string) \
{\
    mvaddstr(row, col, string);\
    clrtoeol();\
    refresh();\
}
#define MENU_ROWS 15
#define MENU_COLS 25
#define OUTPUT_ROW 16
static char main_menu[MENU_ROWS][MENU_COLS] =
{
    "MAIN MENU",
    "(a)dmin functions",
    "(d)esign templates",
    "(f)ab templates",
    "(g)lobal functions",
    "(m)ask sets",
    "(p)c libraries",
    "(q_)uit",
    "\0",
};
static char mask_menu[MENU_ROWS][MENU_COLS] =
{
    "MASK MENU",
    "(c)ancel check-out",
    "(d)elete",
    "(g)rant overwrite",
    "(m)ark checked out",
    "(n)ew PIN",
    "(o)wner change",
    "(p)rint mask directory",
    "(r)etrieve PIN",
    "(s)hadow data",
    "(t)ransfer",
    "(v)ersion delete",
    "(q)uit",
    "\0"
};

static char fab_menu[MENU_ROWS][MENU_COLS] =
{
    "FAB MENU",
    "(c)ancel check-out",
    "(d)elete",
    "(g)rant overwrite",
    "(m)ark checked out",
    "(n)ew PIN",
    "(o)wner change",
    "(p)rint fab directory",
    "(r)etrieve PIN",
    "(s)hadow data",
    "(t)ransfer",
    "(v)ersion delete",
    "(q)uit",
    "\0",
};

static char design_menu[MENU_ROWS][MENU_COLS] =
{
    "DESIGN MENU",
    "(b)uild from mask set",
    "(c)ancel check-out",
    "(d)elete",
    "(g)rant overwrite",
    "(m)ark checked out",
    "(n)ew PIN",
    "(o)wner change",
    "(p)rint design directory",
    "(r)etrieve PIN",
    "(s)hadow data",
    "(t)ransfer",
    "(v)ersion delete",
    "(q)uit",
    "\0"
};

static char pc_menu[MENU_ROWS][MENU_COLS] =
{
    "PC MENU",
    "(d)elete",
    "(n)ew PIN",
    "(o)wner change",
    "(p)rint PC directory",
    "(r)etrieve PIN",
    "(t)ransfer",
    "(q)uit",
    "\0"
};

/* System administrative menu */
static char db_admin_menu[MENU_ROWS][MENU_COLS] =
{
    "DB ADMIN FUNCTIONS",
    "Change (p)assword",
    "Modify (l)ogin name",
    "(q)uit",
    "\0"
};

static char global_menu[MENU_ROWS][MENU_COLS] =
{
    "GLOBAL MENU",
    "Add (a)pprover",
    "Add (b)uilder",
    "(c)ombo - approve/build",
    "(f)orce register user",
    "(m)anual register user",
    "(p)ut global file",
    "List (t)emplate owners",
    "Archive (1) mask set",
    "C(o)mplete archive",
    "(u)narchive one mask set",
    "(q)uit",
    "\0"
};
