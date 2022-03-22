# This makefile builds a library trans_lib/ used by Intercom External
# 'shared_intd_lib.a'


# Include headers
SSL_HDR 	= /usr/include
TRANS_HDR 	= ../trans_include
REG_HDR 	= ../reg_include
GLOBAL_HDR 	= ../global_include
SQL_HDR 	= /run/pkg/TWW-mysql-/5.0.81/include


# Configuration directives
ifeq "$(TRANS_DEBUG)" "YES"
DEBUG_OPTION = -DDEBUG
endif
SWAP_OPTION		= BYTESWAP
SSL_OPTION		= SSL_ENCRYPTION
COMPANY 		= EXTERNAL
PROGRAM			= INTERCOM
MACHINE 		= LINUX64
MISC			= DMZ


# Compiler Flags
CFLAGS = $(DEBUG_OPTION) -D$(SWAP_OPTION) -D$(SSL_OPTION) -D$(COMPANY) -D$(PROGRAM) -D$(MACHINE) -D$(MISC) \
		 -I$(SSL_HDR) -I$(TRANS_HDR) -I$(REG_HDR) -I$(GLOBAL_HDR) -I$(SQL_HDR) -c \
		 -D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64


# Compiler
ifeq "$(TRANS_DEBUG)" "YES"
CC = gcc -g -o3 -Wall
else
CC = gcc -o3 -Wall
endif

# Mazieri's global debug
GLOBAL_DEBUG = ../global_code/global_debug.o

# Files at server_lib/ dir
OBJECT = transweb_reg.o block_put.o trans_sql_lib.o get_release_file.o dlt_history_files.o put_parallel_data.o support.o \
		 trans1step_upload.o put_msg.o dlt_time_calc.o mod_big_file.o ssl_server.o history_big_file.o block_get.o file_type_get.o \
		 dlt_big_file.o get_parallel_data.o status_big_file.o keyword_read.o company.o rmv_dups.o read_global_params.o \
		 dlt_keyword_entries.o send_secure_keyword.o query_user.o report_log.o mod_lock_cnt.o transweb_keyword.o put_success_msg.o \
		 set_trans_disk_usage.o files.o trans1step_download.o search_email_file.o disk_usage_check.o dlt_old_sites.o \
		 extend_big_file.o dlt_empty_global_files.o size_check_big_file.o shared_memory.o reg.o shut_down.o keyword.o \
		 get_random_index.o report_gen.o get_release.o files_dlt_transient.o owner_dlt.o ssl_init.o request.o check_disk_space.o \
		 transweb.o cron_dlt_files.o keyword_send.o log.o time_calc.o transdirlist.o ssl_rmt_lib.o server.o trans_server_globals.o \
		 lookup_keyword.o rcv_file.o


ifeq "$(TRANS_DEBUG)" "YES"
OBJECT += $(GLOBAL_DEBUG)
endif


shared_intd_lib.a: $(OBJECT)
	@echo
	@echo ":: ssl64icommake.mk --target='shared_intd_lib.a' --output='shared_int_lib.a' (External Intercom)"
	@echo
	ar r ../shared_intd_lib.a $(OBJECT)
	ranlib ../shared_intd_lib.a


.DEFAULT: shared_intd_lib.a
