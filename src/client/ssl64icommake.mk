# This makefile builds a library called 'libtrans.a' and used by
# the Intercom External client, target called 'ssl64icom'


# Configuration directives
ifeq "$(TRANS_DEBUG)" "YES"
	DEBUG_OPTION = -DDEBUG
endif

EXTRA_OPTION 	 = TRANSAPI
SWAP_OPTION 	 = BYTESWAP
SERVER_OPTION	 = SINGLE_CONNECT
MISC			 = SINGLE_CONNECT
COMPANY			 = EXTERNAL
PROGRAM			 = INTERCOM
TAR_OPTION 		 = do_tar
MACHINE			 = LINUX64
FOUNDRY_OPTION	 = NONE
SSL_OPTION		 = SSL_ENCRYPTION
TRANSAPI_VERSION = TRANSAPI_VERSION=\"9.1.2\" # TODO: read it from a config file.


# Include Headers
SSL_HDR	 	= /usr/local/ssl/include
TRANS_HDR 	= ../trans_include
GLOBAL_HDR 	= ../global_include
MASK_HDR 	= ../m2include
ZLIB_HDR 	= ../zlib
SQL_HDR 	= /run/pkg/TWW-mysql-/5.0.81/include


# Compiler
ifeq "$(TRANS_DEBUG)" "YES"
	CC = gcc -g -o3 -Wall
else
	CC = gcc -o3 -Wall
endif


# Compiler Flags
CFLAGS =  $(DEBUG_OPTION) -D$(EXTRA_OPTION) -D$(SWAP_OPTION) -D$(SERVER_OPTION) -D$(MISC) -D$(COMPANY) -D$(PROGRAM) -D$(TAR_OPTION) \
		  -D$(FOUNDRY_OPTION) -D$(MACHINE) -D$(SSL_OPTION) -D$(TRANSAPI_VERSION) -I$(SSL_HDR) -I$(TRANS_HDR) -I$(GLOBAL_HDR) -I$(MASK_HDR) -I$(ZLIB_HDR) -I$(SQL_HDR) -c \
		  -D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64


SSL_OBJ = ssl_client_lib.o ssl_common.o ssl_init.o


TRANSAPI_OBJ = close_down.o	chk_password_name.o core_id_crypt.o	dir.o trans_disk_usage.o \
	 		   free_help_print.o files.o get_file.o get_log_string.o trans_intercom_send.o is_keyword.o  \
			   single_help_print.o shared_memory.o shared_status.o trans_block_retrieve.o \
			   trans_gzio.o trans_repository.o trans_file_get.o trans_release_get.o \
			   trans_custom.o trans_get.o trans_files.o transapi.o transapi_client_globals.o transapi_help_print.o \
    		   trans_block_get.o trans_block_put.o parallel_get.o parallel_put.o trans_edc_request.o trans_dirlist.o \
    		   trans_maskshop.o trans_put.o trans_simple.o trans_support.o trans_rcread.o version_display.o whereis.o


# Mazieri's global debug
GLOBAL_DEBUG = ../global_code/global_debug.o


GEN_OBJ = client_lib.o echo_stars.o email_send.o get_ret_code.o getwd.o global_sql_lib.o trans_hash.o init_socket.o \
		  ip_addr.o line_get.o list.o password_crypt.o password.o send2foundry.o scan.o strcpy.o time_calc.o

ifeq "$(TRANS_DEBUG)" "YES"
	GEN_OBJ += $(GLOBAL_DEBUG)
endif


ifeq "$(TRANS_DEBUG)" "YES"
	OUTPUT = icom.debug
else
	OUTPUT = icom
endif

.DEFAULT: ssl64icom

ssl64icom: libtrans.a trans_client.o
	@echo
	@echo ":: ssl64icommake.mk --target='ssl64icom' --output='$(OUTPUT)' (External Intercom Client)"
	@echo
	$(CC) trans_client.o \
	../libtrans.a \
	/usr/lib64/libz.a \
	/run/pkg/TWW-libopenssl-/1.0.0e/lib/libssl.a \
    /run/pkg/TWW-libopenssl-/1.0.0e/lib/libcrypto.a \
	-lkeyutils -lm -ldl -lpthread -o $(OUTPUT) 
	cp --verbose $(OUTPUT) ../

libtrans.a: $(TRANSAPI_OBJ) $(SSL_OBJ) $(GEN_OBJ)
	@echo
	@echo ":: ssl64icommake.mk --target='libtrans.a' --output='libtrans.a' (External Intercom)"
	@echo
	ar r ../libtrans.a $(TRANSAPI_OBJ) $(SSL_OBJ) $(GEN_OBJ)
	ranlib ../libtrans.a
