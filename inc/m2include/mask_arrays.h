#ifndef mask_arrays_h
#define mask_arrays_h
/* Arrays used by both the Mask Prep server and the Mask Prep client */
/* Holds the suffix names for the required files */
    static char required_files[13][15] = {
    ".info",	/* First required design file */
    ".+despat+",	/* First required mask file */
    ".cats",
    ".globldes",
    ".keyword",
    ".maskap",
    ".create",	/* First required fab file */
    ".MaskPrepState",
    ".history",	/* Last required design file */
    ".+fabpat+",
    ".globlfab",
    ".+ordmsk+",	/* Last required mask file */
    ".info"	/* Last required fab file */
};

/* Index into above array for various data types */
static short required_index[3][2] =
{
    1, 12, 	/* Mask */
    0, 9,	/* Design */
    6, 13,	/* Fab */
};
#endif /* ifdef for mask_arrays_h */
