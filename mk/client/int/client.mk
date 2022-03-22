# This makefile builds a library called 'libtrans.a' and used by
# the Intercom Internal client, target called 'ssl64icom'
# producing icom CLI

DIR		=	src/client/


EXTRA_OPTION		= TRANSAPI
SWAP_OPTION			= BYTESWAP
SERVER_OPTION		= MULTIPLE_INTERNAL_SERVERS
SSL_OPTION			= SSL_ENCRYPTION
MISC				= BACKUP_INTERNAL_SERVERS
COMPANY				= NXP
PROGRAM				= INTERCOM
TAR_OPTION 		 	= do_tar
SSL_OPTION		 	= SSL_ENCRYPTION

# Include Headers

TRANS_HDR 		= inc/trans_include
GLOBAL_HDR 		= inc/global_include
MASK_HDR 		= inc/m2include
ZLIB_HDR 		= inc/zlib


include mk/cfg/libraries.def

# Compiler Flags
CFLAGS =  $(DEVEL) $(DEBUG_OPTION) -D$(EXTRA_OPTION) -D$(SWAP_OPTION) -D$(SERVER_OPTION) -D$(MISC)  -D$(COMPANY) -D$(PROGRAM) -D$(TAR_OPTION) \
		  -D$(MACHINE) -D$(SSL_OPTION)   -I$(TRANS_HDR) -I$(GLOBAL_HDR) -I$(MASK_HDR) -I$(ZLIB_HDR) \
		  $(SSL_INCLUDE) \
		  -D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64 $(EXTERNAL_TRANSCEND_ENV_CFLAGS)




# Objects
OBJECT = close_down.o chk_password_name.o core_id_crypt.o ssl_client_lib.o ssl_common.o ssl_init.o \
		  dir.o trans_disk_usage.o free_help_print.o files.o get_file.o get_log_string.o  \
		  client_lib.o echo_stars.o email_send.o get_ret_code.o getwd.o trans_hash.o init_socket.o ip_addr.o \
		  line_get.o list.o password_crypt.o password.o  scan.o strcpy.o time_calc.o \
		  trans_intercom_send.o is_keyword.o single_help_print.o shared_memory_as_mapped_file.o mapped_file.o  shared_status.o \
		  trans_block_retrieve.o trans_gzio.o trans_repository.o trans_file_get.o trans_release_get.o \
		  trans_custom.o trans_get.o trans_files.o transapi.o transapi_client_globals.o transapi_help_print.o \
    	  trans_block_get.o trans_block_put.o parallel_get.o parallel_put.o trans_edc_request.o trans_dirlist.o \
    	  trans_maskshop.o trans_put.o trans_simple.o trans_support.o trans_rcread.o version_display.o whereis.o

OBJECT := $(addprefix $(DIR), $(OBJECT))


include mk/cfg/generic.def


OUTPUT = bin/int.client

## generates icom.int, to release it must be renamed to "icom"

# include(s) below will set the OUTPUT according to some variables
include mk/cfg/output_client_localhost.def
include mk/cfg/output_debug.def

$(info BEGIN)
$(info $(OBJECT))
$(info END)


# Targets
.DEFAULT: ssl64icom


ssl64icom: libtrans.a ./client/trans_client.o
	@echo
	@echo ":: ssl64make.mk --target='ssl64icom' --output='$(OUTPUT)' (Internal Intercom Client)"
	@echo
	$(CC) ./client/trans_client.o ./libtrans.a $(SSL_LIBS) -lz -lm -ldl -lpthread -o ./$(OUTPUT)


libtrans.a: $(OBJECT)
	@echo
	@echo ":: ssl64make.mk --target='libtrans.a' --output='libtrans.a' (Internal Intercom)"
	@echo
	ar r ./libtrans.a $(OBJECT)
	ranlib ./libtrans.a
