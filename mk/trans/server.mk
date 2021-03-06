#
# This makefile builds the SSL Transcend server for RHEL5 64bits.
#

DIR		=	src/transcend/



OS 			 = Linux
SSL_OPTION   = SSL_ENCRYPTION
PROGRAM      = TRANSCEND
COMPANY      = NXP
EDC_OPTION 	 = NO_DMZ

# Headers
INC_PREFIX      =       inc/
GLOBAL_HDR 	= 	$(INC_PREFIX)/global_include
TRANS_HDR       = 	$(INC_PREFIX)/trans_include
REG_HDR 	= 	$(INC_PREFIX)/reg_include
Z_HDR 			= $(INC_PREFIX)/zlib/
SRC_HDR			= ./src/

include mk/cfg/libraries.def


# Compiler and Flags
CFLAGS = $(DEVEL) $(DEBUG_OPTION) -D$(EDC_OPTION) -D$(MACHINE) -D$(COMPANY) -D$(PROGRAM) \
		 -I$(GLOBAL_HDR) -I$(TRANS_HDR) -I$(REG_HDR) -I$(Z_HDR) $(SQL_INCLUDE) -I$(SRC_HDR) $(SSL_INCLUDE) \
		 $(LDAP_LIB) $(SQL_LIB) -D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64 $(EXTERNAL_TRANSCEND_ENV_CFLAGS)


include mk/cfg/generic.def

# OBJECTs
OBJECT = reg.o reg_query.o block_retrieve.o files.o list_dtms_test_files.o ctype.o \
		 get_big_file.o info_big_file.o gzio.o hash.o lookup_release_keyword.o \
		 login_decrypt.o open_big_file.o owner_dlt.o process_request.o put_big_file.o \
		 send_rtv_email.o send_file.o sql_lib.o

OBJECT	:= 	$(addprefix $(DIR), $(OBJECT))

include mk/cfg/generic.def

# Mazieri's LDAP
OBJECT += ./src/ldap/ldapfsl.o src/ldap/ldapfsl_trans.o

OUTPUT = bin/tran.srv
# include(s) below will set the OUTPUT according to some variables
include mk/cfg/output_debug.def

$(info BEGIN)
$(info $(OBJECT))
$(info END)


# SSL Linux Transcend
$(OUTPUT):  $(OBJECT)
	mkdir -p bin
	@echo
	@echo ":: server.mk --target='trans.srv' --output='$(OUTPUT)' (Transcend Server)"
	@echo
	$(CC) $(CFLAGS) $(OBJECT) \
	./lib/shared_trans_lib.a \
	./lib/trans_server_lib.a \
	./lib/trans_reg_lib.a \
	$(SQL_LIB) $(LDAP_LIBS) $(SSL_LIBS) \
	 -lz -lm -ldl -o ./$(OUTPUT)


.DEFAULT: $(OUTPUT)
