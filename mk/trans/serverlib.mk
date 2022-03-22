# This makefile builds a library server_lib/ used by Intercom External
# 'trans_server_lib.a'

DIR		=	server_lib/


# Flags
SSL_OPTION		= SSL_ENCRYPTION
SWAP_OPTION		= BYTESWAP
PROGRAM			= TRANSCEND
COMPANY 		= NXP

# Include headers
TRANS_HDR 	= ./trans_include
REG_HDR 	= ./reg_include
GLOBAL_HDR 	= ./global_include
SRC_HDR		= ./src/

include mk/cfg/libraries.def

# Compiler Flags
CFLAGS = $(DEVEL) $(DEBUG_OPTION) -D$(SSL_OPTION) -D$(MACHINE) -D$(SWAP_OPTION) -D$(PROGRAM) -D$(COMPANY) \
		 -I$(TRANS_HDR) -I$(REG_HDR) -I$(GLOBAL_HDR) $(SQL_INCLUDE) -I$(SRC_HDR) $(SSL_INCLUDE) \
		 -D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64 $(EXTERNAL_TRANSCEND_ENV_CFLAGS)



# Files at server_lib/ dir
OBJECT = entry.o add_line.o close_down.o check_owner.o big_disk_usage.o file.o files.o \
		 dbsend_file.o dbsend_files.o disk_usage.o files_get.o files_put.o dir.o email_send.o \
		 list.o lock_file.o file_append.o file_copy.o file_rename.o files_copy.o files_dlt.o \
		 file_rcv.o files_list.o files_rcv.o  get_random.o get_site.o ssl_common.o \
		 ssl_server_lib.o ssl_utils.o line_get.o mask_msg_server_send.o init_socket.o server_lib.o \
		 log.o names_build.o password_crypt.o password_encrypt.o report_gen.o remote_email_send.o \
		  send_files.o sort.o global_sql_lib.o strcpy.o time_stamp.o sql_keyword_mapping.o sql_destinations_mapping.o create_secure_keyword.o

OBJECT  := $(addprefix $(DIR),$(OBJECT))

include mk/cfg/generic.def


# Mazieri's LDAP
OBJECT += ./src/ldapfsl.o src/ldapfsl_trans.o

$(info BEGIN)
$(info $(OBJECT))
$(info END)


# SSL Transcend server lib
trans_server_lib.a: $(OBJECT)
	@echo
	@echo ":: serverlib.mk --target='trans_server_lib.a' --output='lib/trans_server_lib.a' (Transcend)"
	@echo
	mkdir -p lib
	ar r ./lib/trans_server_lib.a $(OBJECT)
	ranlib ./lib/trans_server_lib.a


.DEFAULT: lib/trans_server_lib.a
