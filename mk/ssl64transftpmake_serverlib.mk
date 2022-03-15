# This makefile builds a library server_lib/ used by TransFTP
# 'free_server_lib.a'

DIR		=	server_lib/

# Include headers
SRC_HDR		= 	./src/
GLOBAL_HDR 	=	./global_include
REG_HDR 	=	./reg_include
TRANS_HDR 	=	./trans_include
SRC_HDR		= 	./src/

include libraries.def


#DEBUG_OPTION = NO_DEBUG
SSL_OPTION		= SSL_ENCRYPTION
SWAP_OPTION		= BYTESWAP
PROGRAM			= INTERCOM
COMPANY			= NXP


# Set up for Linux compile
CFLAGS = $(DEBUG_OPTION) -D$(SSL_OPTION) -D$(MACHINE) -D$(SWAP_OPTION) -D$(PROGRAM) -D$(COMPANY) \
		 $(SSL_INCLUDE) -I$(TRANS_HDR) -I$(REG_HDR) -I$(GLOBAL_HDR) $(SQL_INCLUDE) $(SSL_INCLUDE) \
		 -I$(SRC_HDR) -D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64 $(EXTERNAL_TRANSCEND_ENV_CFLAGS)


OBJECT = entry.o add_line.o close_down.o check_owner.o big_disk_usage.o file.o files.o \
		 dbsend_file.o dbsend_files.o disk_usage.o files_get.o files_put.o \
		 dir.o email_send.o	list.o lock_file.o file_append.o file_copy.o file_rename.o files_copy.o \
		 files_dlt.o file_rcv.o files_list.o files_rcv.o  get_random.o get_site.o  \
		 line_get.o mask_msg_server_send.o init_socket.o ip_addr.o server_lib.o log.o names_build.o \
		 password_crypt.o password_encrypt.o report_gen.o remote_email_send.o \
		  send_files.o sort.o global_sql_lib.o strcpy.o \
		 time_stamp.o ssl_common.o ssl_server_lib.o ssl_utils.o sql_keyword_mapping.o sql_destinations_mapping.o create_secure_keyword.o

OBJECT  := $(addprefix $(DIR),$(OBJECT))

include generic.def

# Mazieri's LDAP
OBJECT += ./src/ldapfsl.o src/ldapfsl_trans.o

$(info BEGIN)
$(info $(OBJECT))
$(info END)


free_server_lib.a: $(OBJECT)
	@echo
	@echo ":: ssl64transftpmake_serverlib.mk --target='free_server_lib.a' --output='free_server_lib.a' (TransFTP)"
	@echo
	ar r ./free_server_lib.a $(OBJECT)
	ranlib ./free_server_lib.a


.DEFAULT: free_server_lib.a
