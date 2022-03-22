# This makefile builds a library trans_lib/ used by Transcend
# and named 'shared_trans_lib.a'

DIR		=	trans_lib/


# Flags
SWAP_OPTION	= 	BYTESWAP
SSL_OPTION	= 	SSL_ENCRYPTION
COMPANY		=	NXP
PROGRAM		=	TRANSCEND
MISC		=	NO_DMZ

# Include headers
TRANS_HDR 	= ./trans_include
REG_HDR 	= ./reg_include
GLOBAL_HDR 	= ./global_include
SRC_HDR		= ./src/

include mk/cfg/libraries.def


# Compiler Flags
CFLAGS = $(DEVEL) $(DEBUG_OPTION) -D$(SWAP_OPTION) -D$(SSL_OPTION) -D$(COMPANY) -D$(PROGRAM) -D$(MACHINE) -D$(MISC) \
	 	 $(SQL_INCLUDE) -I$(GLOBAL_HDR) -I$(TRANS_HDR) -I$(REG_HDR) -I$(SRC_HDR) \
		 -D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64 $(EXTERNAL_TRANSCEND_ENV_CFLAGS)




# Files at trans_lib/ dir
OBJECT = get_random_index.o check_disk_space.o company.o cron_dlt_files.o dlt_empty_global_files.o \
		 dlt_history_files.o  dlt_time_calc.o disk_usage_check.o dlt_big_file.o dlt_keyword_entries.o \
		 extend_big_file.o file_type_get.o files_dlt_transient.o files.o get_release_file.o \
		 get_release.o keyword.o keyword_send.o keyword_read.o log.o lookup_keyword.o mod_lock_cnt.o \
		 get_parallel_data.o block_get.o block_put.o put_parallel_data.o shared_memory.o history_big_file.o \
		 mod_big_file.o owner_dlt.o put_msg.o put_success_msg.o request.o reg.o read_global_params.o report_gen.o \
		 report_log.o rmv_dups.o rcv_file.o server.o ssl_server.o status_big_file.o search_email_file.o shut_down.o \
		 size_check_big_file.o ssl_rmt_lib.o ssl_init.o support.o send_secure_keyword.o set_trans_disk_usage.o \
		 trans_server_globals.o trans1step_upload.o trans1step_download.o trans_sql_lib.o time_calc.o transweb.o \
		 transweb_keyword.o transdirlist.o transweb_reg.o query_user.o

OBJECT := $(addprefix $(DIR),$(OBJECT))

include mk/cfg/generic.def


# Mazieri's LDAP
OBJECT += ./src/ldapfsl.o src/ldapfsl_trans.o

$(info BEGIN)
$(info $(OBJECT))
$(info END)


# Transcend library
lib/shared_trans_lib.a: $(OBJECT)
	@echo
	@echo ":: ssl64transmake_translib.mk --target='lib/shared_trans_lib.a' --output='lib/shared_trans_lib.a' (Transcend)"
	@echo
	mkdir -p lib
	ar r ./lib/shared_trans_lib.a $(OBJECT)
	ranlib ./lib/shared_trans_lib.a


.DEFAULT: lib/shared_trans_lib.a
