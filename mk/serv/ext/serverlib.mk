# This makefile builds a library server_lib/ used by Intercom External
# 'free_server_lib.a'

DIR		=	server_lib/

# Include headers
TRANS_HDR 	= 	./trans_include
REG_HDR 	= 	./reg_include
GLOBAL_HDR 	= 	./global_include
SRC_HDR		= 	./src/

include mk/cfg/libraries.def


SSL_OPTION		= SSL_ENCRYPTION
SWAP_OPTION		= BYTESWAP
PROGRAM			= INTERCOM
COMPANY 		= EXTERNAL


# Compiler Flags
CFLAGS = $(DEVEL) $(DEBUG_OPTION) -D$(SSL_OPTION) -D$(MACHINE) -D$(SWAP_OPTION) -D$(PROGRAM) -D$(COMPANY) \
		 $(SSL_INCLUDE) -I$(TRANS_HDR) -I$(REG_HDR) -I$(GLOBAL_HDR) $(SQL_INCLUDE) -I$(SRC_HDR) \
		 -D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64 $(EXTERNAL_TRANSCEND_ENV_CFLAGS)


# Files at server_lib/ dir
OBJECT = check_owner.o global_sql_lib.o ssl_common.o file_append.o file_copy.o files_dlt.o  \
		 names_build.o  file_rename.o ssl_utils.o get_random.o files_get.o add_line.o \
		 close_down.o send_files.o files_put.o dbsend_files.o mask_msg_server_send.o files_rcv.o file.o \
		 password_encrypt.o lock_file.o sort.o password_crypt.o email_send.o files_list.o server_lib.o \
		 disk_usage.o big_disk_usage.o ssl_server_lib.o files_copy.o files.o strcpy.o \
		 get_site.o init_socket.o report_gen.o line_get.o entry.o list.o dir.o log.o file_rcv.o \
		 dbsend_file.o remote_email_send.o sql_keyword_mapping.o sql_destinations_mapping.o create_secure_keyword.o

OBJECT  := $(addprefix $(DIR),$(OBJECT))

include mk/cfg/generic.def


# New LDAP
OBJECT += ./src/ldapfsl.o src/ldapfsl_trans.o

$(info BEGIN)
$(info $(OBJECT))
$(info END)

lib/free_server_lib.a: $(OBJECT)
	@echo
	@echo ":: ssl64icommake_serverlib.mk --target='lib/free_server_lib.a' --output='lib/free_server_lib.a' (External Intercom)"
	@echo
	ar r ./lib/free_server_lib.a $(OBJECT)
	ranlib ./lib/free_server_lib.a


.DEFAULT: lib/free_server_lib.a
