# This makefile builds a library called 'libtrans.a' and used by
# the Intercom External client, target called 'ssl64icom'

DIR		=	client/

EXTRA_OPTION 	 = TRANSAPI
SWAP_OPTION 	 = BYTESWAP
SERVER_OPTION	 = SINGLE_CONNECT
MISC			 = SINGLE_CONNECT
COMPANY			 = EXTERNAL
PROGRAM			 = INTERCOM
TAR_OPTION 		 = do_tar
FOUNDRY_OPTION	 = NONE
SSL_OPTION		 = SSL_ENCRYPTION

# Include Headers

TRANS_HDR 	= ./trans_include
GLOBAL_HDR 	= ./global_include
MASK_HDR 	= ./m2include
ZLIB_HDR 	= ./zlib

include libraries.def

# Compiler Flags
CFLAGS =  $(DEBUG_OPTION) -D$(EXTRA_OPTION) -D$(SWAP_OPTION) -D$(SERVER_OPTION) -D$(MISC) -D$(COMPANY) -D$(PROGRAM) -D$(TAR_OPTION) \
		  -D$(FOUNDRY_OPTION) -D$(MACHINE)    -I$(TRANS_HDR) -I$(GLOBAL_HDR) -I$(MASK_HDR) -I$(ZLIB_HDR)  -c \
		  $(SSL_INCLUDE) -D$(SSL_OPTION) \
		  -D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64 -DLDAP_DEPRECATED=1 $(EXTERNAL_TRANSCEND_ENV_CFLAGS)



OBJECT = close_down.o chk_password_name.o core_id_crypt.o dir.o trans_disk_usage.o \
	 	  free_help_print.o files.o get_file.o get_log_string.o trans_intercom_send.o is_keyword.o  \
		  single_help_print.o shared_memory_as_mapped_file.o mapped_file.o shared_status.o trans_block_retrieve.o \
		  trans_gzio.o trans_repository.o trans_file_get.o trans_release_get.o ssl_client_lib.o ssl_common.o ssl_init.o \
		  trans_custom.o trans_get.o trans_files.o transapi.o transapi_client_globals.o transapi_help_print.o \
    	  trans_block_get.o trans_block_put.o parallel_get.o parallel_put.o trans_edc_request.o trans_dirlist.o \
    	  trans_maskshop.o trans_put.o trans_simple.o trans_support.o trans_rcread.o version_display.o whereis.o \
		  client_lib.o echo_stars.o email_send.o get_ret_code.o getwd.o trans_hash.o init_socket.o \
		  ip_addr.o line_get.o list.o password_crypt.o password.o  scan.o strcpy.o time_calc.o

OBJECT := $(addprefix $(DIR), $(OBJECT))

include generic.def

$(info BEGIN)
$(info $(OBJECT))
$(info END)

OUTPUT = bin/ext.client
## generates icom.ext, to release it must be renamed to "icom"

# include(s) below will set the OUTPUT according to some variables
include output_client_localhost.def
include output_debug.def


.DEFAULT: ssl64icom

ssl64icom: libtrans.a ./client/trans_client.o
	@echo
	@echo ":: ssl64icommake.mk --target='ssl64icom' --output='$(OUTPUT)' (External Intercom Client)"
	@echo
	$(CC) ./client/trans_client.o \
	./libtrans.a \
	$(SSL_LIBS) \
	-lz -lm -ldl -lpthread -o $(OUTPUT)

libtrans.a: $(OBJECT)
	@echo
	@echo ":: ssl64icommake.mk --target='libtrans.a' --output='libtrans.a' (External Intercom)"
	@echo
	ar r ./libtrans.a $(OBJECT)
	ranlib ./libtrans.a
