#
# This makefile builds the SSL Transcend client for RHEL6 64bits.
#

DIR		=	src/client/


EXTRA_OPTION	=	TRANSAPI
SWAP_OPTION		=	BYTESWAP
SERVER_OPTION	=	MULTIPLE_INTERNAL_SERVERS
MISC			=	BACKUP_INTERNAL_SERVERS
COMPANY			=	NXP
PROGRAM			=	TRANSCEND
TAR_OPTION		= 	do_tar
#FOUNDRY_OPTION		=	NONE
SSL_OPTION		=	SSL_ENCRYPTION

## SOLARIS -> remember to run:
##
##	$ setenv _WRAPPER_ /proj/dmisw/transdev/work/b36283/SVNs/transcend/branches/NXP/solaris.policy
##
## And in case of need a trace_debug.log run also:
##
## 	$ setenv TRANS_DEBUG YES
##	$ setenv TRANS_DEVEL YES
##
## Then for Solaris, alternate here



# Headers
TRANS_HDR 	= ./inc/trans_include
GLOBAL_HDR 	= ./inc/global_include
MASK_HDR 	= ./inc/m2include
ZLIB_HDR 	= ./inc/zlib

include mk/cfg/libraries.def




# Compiler flags
CFLAGS = $(DEVEL) $(DEBUG_OPTION) -D$(EXTRA_OPTION) -D$(SWAP_OPTION) -D$(SERVER_OPTION) -D$(MISC) -D$(COMPANY) -D$(PROGRAM) -D$(TAR_OPTION) \
		 -D$(MACHINE) -D$(SSL_OPTION)  $(SSL_INCLUDE) -I$(TRANS_HDR) -I$(GLOBAL_HDR) -I$(MASK_HDR) -I$(ZLIB_HDR) \
		 -D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64 -DLDAP_DEPRECATED=1 $(EXTERNAL_TRANSCEND_ENV_CFLAGS)




OBJECT = close_down.o chk_password_name.o core_id_crypt.o dir.o trans_disk_usage.o free_help_print.o files.o get_file.o get_log_string.o  \
		trans_intercom_send.o is_keyword.o single_help_print.o shared_memory_as_mapped_file.o mapped_file.o shared_status.o trans_block_retrieve.o \
		trans_gzio.o trans_repository.o trans_file_get.o trans_release_get.o trans_custom.o trans_get.o trans_files.o \
		transapi.o transapi_client_globals.o transapi_help_print.o trans_block_get.o trans_block_put.o parallel_get.o parallel_put.o \
		trans_edc_request.o trans_dirlist.o trans_maskshop.o trans_put.o trans_simple.o trans_support.o trans_rcread.o version_display.o whereis.o \
		ssl_client_lib.o ssl_common.o ssl_init.o client_lib.o echo_stars.o email_send.o get_ret_code.o getwd.o trans_hash.o \
		init_socket.o ip_addr.o line_get.o list.o password_crypt.o password.o  scan.o strcpy.o time_calc.o

OBJECT := $(addprefix $(DIR), $(OBJECT))


include mk/cfg/generic.def


OUTPUT = bin/tran.client
# include(s) below will set the OUTPUT according to some variables
include mk/cfg/output_client_localhost.def
include mk/cfg/output_debug.def

$(info BEGIN)
$(info $(OBJECT))
$(info END)


.DEFAULT: lib/libtrans.a


# BUILD
$(OUTPUT): lib/libtrans.a ./src/client/trans_client.o
	@echo
	@echo ":: client.mk --target='lib/libtrans.a' --output='$(OUTPUT)' (Transcend Client)"
	@echo
	@echo $(OUTPUT)
	$(LINK) ./src/client/trans_client.o ./lib/libtrans.a \
	$(SSL_LIBS) \
	-lz -lm -ldl -o ./$(OUTPUT)


lib/libtrans.a: $(OBJECT)
	@echo
	@echo ":: client.mk --target='lib/libtrans.a' --output='libtrans.a' (Transcend)"
	@echo
	ar r ./lib/libtrans.a $(OBJECT)
	ranlib ./lib/libtrans.a
