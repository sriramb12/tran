#ifndef mask_client_defines_h
#define  mask_client_defines_h

/* Holds special Mask status flags */
typedef struct
{
    int info_flag;
    int dsgn_file_num;
    int tmpl_file_num;
    char *dsgn_info_ptr;
    char *wl_info_ptr;
    int file_offset;
    int tmp_file_offset;
} MASK_CLIENT_FLAGS;

#endif /* define for mask_clikent_defines_h */
