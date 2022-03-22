#ifndef server_caution_errs_h
#define server_caution_errs_h
/* Caution messages */
#define mask_read_cau "\n\nCaution -470:  While listing masks unable to read state file:"
#define MASK_READ_CAU -470
#define rd_open_cau "\n\nCaution -471:  Unable to open for read file '"
#define RD_OPEN_CAU -471
#define connect_cau "\n\nCaution -472:  Received error code '%d' while trying to connect to remote server '%s'"
#define CONNECT_CAU -472
#define CONNECT_CAU -472
#define upd_server_cau "\n\nCaution -473:  Unable to find global entry for database server '"
#define UPD_SERVER_CAU -473
#define no_glbl_table_cau "\n\nCaution -474:  Entry for '%s' not present in global table:%s.\n\
               Missing entry added."
#define NO_GLBL_TABLE_CAU -474
#define rd_parent_cau "\n\nCaution -475:  Unable to open for read parent file '"
#define RD_PARENT_CAU -475
#define mkdir_cau "Error -476:  Unable to create directory '"
#define MKDIR_CAU -476
#define mkdir_audit "Audit -474:  Created missing directory '"
#define MKDIR_AUDIT -474
#define wr_open_cau "\n\nCaution -477:  Unable to open for write file '"
#define WR_OPEN_CAU -477
#define rename_cau "\n\nCaution -478:  Unable to rename '%s' to '%s'."
#define RENAME_CAU -478
#define open_dir_cau "\n\nCaution -479:  Unable to open for read directory '"
#define OPEN_DIR_CAU -479
#define read_cau "\n\nCaution -480:  Unable to read file '"
#define READ_CAU -480
#define extra_dir "\n\nCaution -481:    Found and deleted unexpected database directory:\n\
               '"
#define EXTRA_DIR -481
#define bad_toc_cau "\n\nCaution -482:    Unable to find and delete:\n\n\
               Global entry:  "
#define BAD_TOC_CAU -482
#define no_glbl_entry_cau "\n\nCaution -483:  Entry for '%s' not present in global table:%s.\n\
               Missing entry added."
#define NO_GLBL_ENTRY -483
#define global_table_cau "\n\nCaution -484:  Duplicate global table entry found for:\n\n\
               Table:  '%s'\n\
          Entry:  '%s'\n"
#define GLOBAL_TABLE_CAU -484
#define dup_entry_cau "\n\nCaution -485:  Duplicate entry found for:\n\n\
               Global entry:  "
#define DUP_ENTRY_CAU -485
#define invalid_dir_name "\n\nCaution -486:  Unable to delete directory with 0 length name."
#define INVALID_DIR_NAME -486
#define invalid_dir_open "\n\nCaution -487:  Unable to open for deletion directory:\n\
               '"
#define INVALID_DIR_OPEN -487
#define invalid_rmdir "\n\nCaution -488:  Unable to remove directory:\n\
               '"
#define INVALID_RMDIR -488
#define invalid_unlink "\n\nCaution -489:  Unable to unlink file:\n\
               '"
#define INVALID_UNLINK -489
#define no_mask_delta "\n\nCaution -490:  No delta files present for mask set '"
#define NO_MASK_DELTA -490
#define extra_mask_entry "\n\nCaution -491:  Extra entry found in global table for mask set '"
#define EXTRA_MASK_ENTRY -491
#define missing_mask_entry "\n\nCaution -492:  Added missing entry in global table for mask set '"
#define MISSING_MASK_ENTRY -492
#define parse_cau "\n\nCaution -493:  Unable to parse line '%s'\n\
               in file '%s'"
#define PARSE_CAU -493
#define builder_cau "\n\nCaution -494:  %d template builders have been defined.  Database manager only allows %d builders.\n\
              Increase this limit defined in 'pc_data.h' and re-compile the database server."
#define BUILDER_CAU -494
#define index_cau "\n\nCaution -495:  Invalid index found while parsing table '"
#define INDEX_CAU -495
#define mod_state_cau "\n\nCaution -496:  Changed approval state to '%s' in file '%s'"
#define MOD_STATE_CAU -496
#define mod_table_cau "\n\nCaution -497:  Modified approval state for '%s' to '%c' in table '%s'"
#define MOD_TABLE_CAU -497
#define split_cau "\n\nCaution -498:  Unable to split '%s' into fab and flow."
#define SPLIT_CAU -498
#define rmt_update_cau "\n\nCaution 499:  Remote update unable to open for read file "
#define RMT_UPDATE_CAU -499
#define extra_entry_cau "Caution -519:  Extra entry '%s' found and deleted from global table  '%s'"
#define EXTRA_ENTRY_CAU -519

#endif /* ifdef for server_caution_errs_h */
