#ifndef TRANSAPI
/****************************************************************************
 *
 * Name:	trans_client_globals
 *
 * Description:	Global variables used by the Transcend client.
 *
 ***************************************************************************/

#include "company.h"
#include "global_defines.h"
#include "server_params.h"
#include "trans_defines.h"
#include "transapi_defines.h"
#include "rtn.h"
#include "transrc.h"
#include "repositories_defines.h"

# define TRANSWEB_PORT             EXTERNAL_INTERCOM_PORT


/* Holds info about the user */
USER_PARAMS user;

/* Holds info about the server */
SERVER_PARAMS server;

/* Holds return message */
RTN rtn;
/* Holds socket number */
int sock;

struct transrc_t transrc;

DB_SOFTWARE so;
TRANS_FILE_TYPE global_file_type;
char global_keyword[256];
int global_dlt;
char icap_class[8];




#ifdef EXTERNAL_TRANSWEB
TRANS_REPOSITORY repository[10] =
{
    { FREE_COMPANY, 'e', 'E', INTERCOM_ADDR, INTERCOM_IP, TRANSWEB_PORT},
    /* 2nd option for intercom in case first changes its address */
    { FREE_COMPANY, 'e', 'E', MASTER_REP_ADDR,          MASTER_REP_IP,             TRANSCEND_PORT},
    { "",'\0', '\0', "", "", 0},
};

TRANS_COMPANY site[TOTAL_COMPANY_CNT] =
{
    { FREE_COMPANY, TRANSWEB_COMPANY, INTERCOM_ADDR, INTERCOM_IP, STR_P(TRANSWEB_PORT), "Phoenix",  "", "no" },
    { "", "", "", "", STR_P(0), """", },
};
#endif /* EXTERNAL_TRANSWEB */


#if !defined(EXTERNAL_TRANSWEB) // internal transweb, LEGACY no longer used
TRANS_REPOSITORY repository[10] =
{
    { "Austin", 'a', 'A', "austin-trans.am.freescale.net", "10.81.199.101", TRANSWEB_PORT},
    { "WROC", 'w', 'W', "wroc-trans.am.freescale.net", "10.65.24.42", TRANSWEB_PORT},
    { "Tempe", 't', 'T', "tempe-trans.am.freescale.net", "10.65.24.19", TRANSWEB_PORT},
    { "Munich", 'm', 'M', "munich-trans.ea.freescale.net", "10.170.90.60", TRANSWEB_PORT},
    { "Eindhoven", 'i', 'I', "inv0721.nxdi.nl-cdc01.nxp.com", "92.120.72.14", TRANSWEB_PORT},
    { "Bangalore", 'b', 'B', "inv0745.in-blr01.nxp.com", "165.114.116.154", TRANSWEB_PORT},
    { "Noida", 'n', 'N', "india-trans.am.freescale.net", "10.232.5.13", TRANSWEB_PORT},
    { "Israel", 'r', 'R', "fil-trans.am.freescale.net", "223.76.95.11", TRANSWEB_PORT},
    { "Brazil", 'z', 'Z', "brazil-trans.am.freescale.net", "10.29.241.77", TRANSWEB_PORT},
    { "DTMS", 'd', 'D', "dtms-trans-tmp.am.freescale.net", "10.84.132.6", TRANSWEB_PORT},
    { "External", 'e', 'E', INTERCOM_ADDR, INTERCOM_IP, TRANSWEB_PORT},
    { "",'\0', '\0', "", "", 0},
};

TRANS_COMPANY site[TOTAL_COMPANY_CNT] =
{
    { "NXP (Austin)", "aus", "austin-trans.am.freescale.net", "10.81.199.101", STR_P(TRANSWEB_PORT), "Phoenix",  "", "no" },
    { "NXP (WROC)", "wro", "wroc-trans.am.freescale.net", "10.65.24.42", STR_P(TRANSWEB_PORT), "Phoenix",  "", "no" },
    { "NXP (Tempe)", "tem", "tempe-trans.am.freescale.net", "10.65.24.19", STR_P(TRANSWEB_PORT), "Tempe",  "", "no" },
    { "NXP (Munich)", "mun", "munich-trans.ea.freescale.net", "10.170.90.60", STR_P(TRANSWEB_PORT), "Phoenix",  "", "no" },
    { "NXP (Noida)", "idc", "asia-trans.am.freescale.net", "10.232.5.13", STR_P(TRANSWEB_PORT), "Phoenix",  "", "no" },
    { "NXP (Bangalore)", "blr", "inv0745.in-blr01.nxp.com", "165.114.116.154", STR_P(TRANSWEB_PORT), "Bangalore", "", "no"},
    { "NXP (Israel)", "isrl", "blnx11", "223.76.95.11", STR_P(TRANSWEB_PORT)},
    { "NXP (Brazil)", "brzl", "brazil-trans.am.freescale.net", "10.29.241.77", STR_P(TRANSWEB_PORT)},
    { "NXP (DTMS)", "dtms", "dtms-trans-tmp.am.freescale.net", "10.84.132.6", STR_P(TRANSWEB_PORT)},
    { "External TransWeb", "etc", "az84ap33.am.freescale.net", "10.64.2.227", STR_P(TRANSWEB_PORT), "Phoenix",  "", "no" },
    { "CompuGraphics", "cgi", "az84ap33.am.freescale.net", "10.64.2.227", STR_P(TRANSWEB_PORT), "Phoenix", "dp@cgi.co.uk", "no" },
    { "Toppan (Round Rock)", "drr", "az84ap33.am.freescale.net", "10.64.2.227", STR_P(TRANSWEB_PORT), "Phoenix",  "intercom@photomask.com", "no" },
    { "Toppan (Rousset)", "drs", "az84ap33.am.freescale.net", "10.64.2.227", STR_P(TRANSWEB_PORT), "Phoenix",  "rt_order_entry@dupont-photomasks.fr;malglaivei@dupont-photomasks.fr;leonard.perry@photomask.com;sherry.bahrami@photomask.com", "no" },
    { "Toppan (Santa Clara)", "dsh", "az84ap33.am.freescale.net", "10.64.2.227", STR_P(TRANSWEB_PORT), "Phoenix",  "intercom@photomask.com", "no" },
    { "Toppan (Shanghai)", "dsc", "az84ap33.am.freescale.net", "10.64.2.227", STR_P(TRANSWEB_PORT), "Phoenix",  "intercom@photomask.com", "no" },
    { "Mentorgraphics", "mtg", "az84ap33.am.freescale.net", "10.64.2.227", STR_P(TRANSWEB_PORT), "Phoenix",  "", "no" },
    { "Photronics (Allen TX)", "pat", "az84ap33.am.freescale.net", "10.64.2.227", STR_P(TRANSWEB_PORT), "Phoenix",  "dpmot.dis@austin.photronics.com", "no" },
    { "Photronics (Austin TX)", "put", "az84ap33.am.freescale.net", "10.64.2.227", STR_P(TRANSWEB_PORT), "Phoenix",  "dpmot.dis@austin.photronics.com", "no" },
    { "Photronics (Brookfield CT)", "pbc", "az84ap33.am.freescale.net", "10.64.2.227", STR_P(TRANSWEB_PORT), "Phoenix",  "dpmot.dis@austin.photronics.com", "no" },
    { "Photronics (Manchester UK)", "pme", "az84ap33.am.freescale.net", "10.64.2.227", STR_P(TRANSWEB_PORT), "Phoenix",  "mot_icom@uk.photronics.com", "no" },
    { "Photronics (Northern CA Op)", "pnc", "az84ap33.am.freescale.net", "10.64.2.227", STR_P(TRANSWEB_PORT), "Phoenix",  "dpmot.dis@nco.photronics.com", "no" },
    { "Synopsys", "sns", "az84ap33.am.freescale.net", "10.64.2.227", STR_P(TRANSWEB_PORT), "Phoenix",  "", "no" },
    { "", "", "", "", STR_P(0), """", },
};
#endif // !defined(EXTERNAL_TRANSWEB)

/* Holds remote directory name when data sent to TSMC */



/* Holds email address found in company_fine */
char remote_company_email[1024];
char tar_option;
int compression;
char remote_mask[5];
char reg_email1[64];
char global_friendly_email[64];
char remote_dir[FILE_SIZE];
int transnet_compression;
long long global_final_size;
int global_file_cnt;
char global_project[64];

/* Holds file descriptor for open pipe during parallel writes */
int fi;

/* Keeps track of offset during parallel data transfer */
long long total_offset;

/* Holds the id for shared memory */
int shm_id;
int fi_array[10][2];
int global_counter;
int global_dir_offset; /* Offset for directory if slashes are in original file name */
int write_pipe;
int global_dir_cnt;
int log_files;
char tmp_transdirlist[64];
long long global_compressed_size;
float compression_ratio;
char *ratio_list[500];
int size_printed;
char *extension_list[500];
char reg_email[256];
char icomrc_path[PATH_SIZE];
char reg_email2[256];
int perform_background_transfer;
char global_par6[8];
#endif /* For not TRANSAPI */
