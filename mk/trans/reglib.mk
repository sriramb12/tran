# This makefile builds a library used by
# the Transcend server called
# 'free_reg_lib.a'

DIR		=	src/reg_lib/

# Include Headers
INC_PREFIX      =       inc/
GLOBAL_HDR 	= 	$(INC_PREFIX)/global_include
TRANS_HDR       = 	$(INC_PREFIX)/trans_include
REG_HDR 	= 	$(INC_PREFIX)/reg_include

include mk/cfg/libraries.def

SWAP_OPTION 	= BYTESWAP
SSL_OPTION 		= SSL_ENCRYPTION
EDC_OPTION		= NO_DMZ
SERVER_OPTION	= INTERNAL
PROGRAM			= TRANSCEND
COMPANY 		= NXP
SRC_HDR			= ./src/


# Compiler Flags
CFLAGS = $(DEVEL) $(DEBUG_OPTION) -D$(SWAP_OPTION) -D$(SSL_OPTION) -D$(EDC_OPTION) \
		 -D$(SERVER_OPTION) -D$(PROGRAM) -D$(COMPANY) -D$(MACHINE) \
		 -I$(GLOBAL_HDR) -I$(TRANS_HDR) -I$(REG_HDR) $(SQL_INCLUDE) -I$(SRC_HDR) \
		 -D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64 $(EXTERNAL_TRANSCEND_ENV_CFLAGS)


# Files at reg_lib/ dir
OBJECT = search_registry.o add_email_entry.o add_file_entry.o add_file_line.o add_site_entry.o alarm.o \
		 compare_names.o compare_sites.o core_id_crypt.o dlt_file_entry.o dlt_file_line.o dlt_site_entry.o \
		 find_email.o edc_reg.o init_rmt_socket.o  init_ssl_socket.o reg_lookup.o \
		 rmt_server.o rmt_lib.o resend_reg.o ssl_rmt_lib.o sql_reg.o strcmp1err.o strdlt_quotes.o \
		 strip_email_trailors.o user_autoreg.o x500.o

OBJECT  := $(addprefix $(DIR), $(OBJECT))

include mk/cfg/generic.def


# Mazieri's LDAP
OBJECT += ./src/ldap/ldapfsl.o src/ldap/ldapfsl_trans.o

$(info BEGIN)
$(info $(OBJECT))
$(info END)

lib/trans_reg_lib.a: $(OBJECT)
	@echo
	@echo ":: ssl64transmake_reglib.mk --target='lib/trans_reg_lib.a' --output='lib/trans_reg_lib.a' (Transcend)"
	@echo
	ar r ./lib/trans_reg_lib.a $(OBJECT)
	ranlib ./lib/trans_reg_lib.a


.DEFAULT: lib/trans_reg_lib.a
