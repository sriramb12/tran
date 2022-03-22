#ifndef file_type_arrays_h
#define file_type_arrays_h

char binary2array[4][3] =
{ ".o", ".a", ".Z", "" };

char binary3array[3][4] =
{
    ".gz",
    ""
};

char binary4array[20][5] =
{
    ".bif",	/* Graphics file */
    ".dif",	/* Graphics file */
    ".dll",	/* Dynamicly linked library */
    ".doc",	/* Word file */
    ".dot",	/* Works file */
    ".exe",	/* Executable file */
    ".gds",	/* GDS stream file */
    ".gif",	/* Graphics file */
    ".hlp",
    ".ini",	/* Windows file */
    ".mdb",
    ".mid",	/* Midi file */
    ".qdb",	/* Quickan file */
    ".jpg",
    ".tif",	/* Graphics file */
    ".pcx",
    ".obj",	/* Object file */
    ".snd",	/* Sound file */
    ".tar", 	/* Tar file */
    ""
};

char binary5array[5][6] = 
{
    ".cflt", ""
};

char binary6array[5][7] =
{
    ".gdsii", ""
};


/* Text files */
char text2array[6][3] =
{
    ".c", ".f", ".h", ".l",  ".y", ""
};

char text3array[4][4] =
{
    ".cp",  ".ps", ""
};

char text4array[5][5] =
{
    ".bat", ".txt",  ""
};

#endif
