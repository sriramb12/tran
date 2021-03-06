# This makefile builds a library used by the TransFTP server
# called free_reg_lib.a

DIR	= reg_lib/

# Include Headers
GLOBAL_HDR		= ./global_include
TRANS_HDR		= ./trans_include
REG_HDR 		= ./reg_include
SRC_HDR			= ./src/

include libraries.def


PROGRAM 		= INTERCOM
SWAP_OPTION 	= BYTESWAP
#DEBUG_OPTION 	= NO_DEBUG
EDC_OPTION		= NO_DMZ
SERVER_OPTION	= INTERNAL
SSL_OPTION		= SSL_ENCRYPTION
PROGRAM			= TRANS_FTP
COMPANY			= NXP

# Compiler Flags
CFLAGS = $(DEBUG_OPTION) -D$(SWAP_OPTION) -D$(SSL_OPTION) -D$(EDC_OPTION) -D$(SERVER_OPTION) -D$(PROGRAM) -D$(COMPANY) -D$(MACHINE) \
		 $(LDAP_INCLUDE) -I$(GLOBAL_HDR) -I$(TRANS_HDR) -I$(REG_HDR) $(SSL_INCLUDE) $(SQL_INCLUDE) -I$(SRC_HDR) \
		 -D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64 $(EXTERNAL_TRANSCEND_ENV_CFLAGS)




# Files at reg_lib/ dir
OBJECT = search_registry.o add_email_entry.o add_file_entry.o add_file_line.o add_site_entry.o \
		 alarm.o compare_names.o compare_sites.o core_id_crypt.o dlt_file_entry.o dlt_file_line.o dlt_site_entry.o \
		 find_email.o edc_reg.o  init_rmt_socket.o  init_ssl_socket.o \
		 reg_lookup.o rmt_server.o rmt_lib.o resend_reg.o ssl_rmt_lib.o sql_reg.o strcmp1err.o strdlt_quotes.o \
		 strip_email_trailors.o  user_autoreg.o x500.o

OBJECT  := $(addprefix $(DIR), $(OBJECT))

include generic.def


# New LDAP
OBJECT += ./src/ldapfsl.o src/ldapfsl_trans.o

$(info BEGIN)
$(info $(OBJECT))
$(info END)


free_reg_lib.a: $(OBJECT)
	@echo
	@echo ":: ssl64transftpmake_reglib.mk --target='free_reg_lib.a' --output='free_reg_lib.a' (TransFTP)"
	@echo
	ar r ./free_reg_lib.a $(OBJECT)
	ranlib ./free_reg_lib.a


.DEFAULT: free_reg_lib.a
