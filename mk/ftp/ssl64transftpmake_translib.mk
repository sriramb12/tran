# This makefile builds a library trans_lib/ used by TransFTP
# and named 'shared_intd_lib.a'

DIR		=	trans_lib/


# Include headers
GLOBAL_HDR 	=	./global_include
TRANS_HDR 	=	./trans_include
REG_HDR 	=	./reg_include
SRC_HDR		=	./src/

include libraries.def

#DEBUG_OPTION = NO_DEBUG
SWAP_OPTION	=	BYTESWAP
SSL_OPTION	=	SSL_ENCRYPTION
MISC		=	NO_DMZ
PROGRAM		=	TRANS_FTP
COMPANY		=	NXP


# Compiler Flags
CFLAGS =  $(DEBUG_OPTION) -D$(SWAP_OPTION) -D$(SSL_OPTION) -D$(COMPANY) -D$(PROGRAM) -D$(MACHINE) -D$(MISC) \
	 	  $(SSL_INCLUDE) -I$(GLOBAL_HDR)  -I$(TRANS_HDR) -I$(REG_HDR) -I$(SRC_HDR) $(SQL_INCLUDE) \
		  -D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64 $(EXTERNAL_TRANSCEND_ENV_CFLAGS)



OBJECT = get_random_index.o check_disk_space.o company.o cron_dlt_files.o \
		 dlt_empty_global_files.o dlt_history_files.o  dlt_time_calc.o \
		 disk_usage_check.o dlt_big_file.o \
		 dlt_keyword_entries.o dlt_old_sites.o extend_big_file.o file_type_get.o  \
		 files_dlt_transient.o files.o \
		 get_release_file.o get_release.o keyword.o keyword_send.o keyword_read.o log.o lookup_keyword.o mod_lock_cnt.o \
		 get_parallel_data.o block_get.o block_put.o put_parallel_data.o  shared_memory.o \
		 history_big_file.o mod_big_file.o owner_dlt.o \
		 put_msg.o put_success_msg.o  request.o  \
		 reg.o read_global_params.o report_gen.o report_log.o rmv_dups.o \
		 rcv_file.o server.o ssl_server.o status_big_file.o search_email_file.o \
		 shut_down.o size_check_big_file.o \
		 ssl_rmt_lib.o ssl_init.o support.o send_secure_keyword.o set_trans_disk_usage.o\
		 trans_server_globals.o trans1step_upload.o trans1step_download.o \
		  trans_sql_lib.o time_calc.o transweb.o transweb_keyword.o \
		 transdirlist.o transweb_reg.o query_user.o


OBJECT := $(addprefix $(DIR),$(OBJECT))

include generic.def

# Mazieri's LDAP
OBJECT += ./src/ldapfsl.o src/ldapfsl_trans.o

$(info BEGIN)
$(info $(OBJECT))
$(info END)


# TransFTP library
shared_intd_lib.a: $(OBJECT)
	@echo
	@echo ":: ssl64transftpmake_translib.mk --target='shared_intd_lib.a' --output='shared_intd_lib.a' (TransFTP)"
	@echo
	ar r ./shared_intd_lib.a $(OBJECT)
	ranlib ./shared_intd_lib.a

.DEFAULT: shared_intd_lib.a