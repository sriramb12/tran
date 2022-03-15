# This makefile ssl64mmake.mk is called to produce target ssl64icomd
# of the Intercom Internal server

DIR		=	intercom/

CUSTOMER_OPTION 	= DMZ
SSL_OPTION 			= SSL_ENCRYPTION
PROGRAM 			= INTERCOM
COMPANY 			= NXP

include libraries.def

Z_LIB 		= 	./zlib
LDAP_LIB	= 	/usr/lib64

# Include Headers
GLOBAL_HDR 	= 	./global_include
TRANS_HDR       = 	./trans_include
REG_HDR 	= 	./reg_include
Z_HDR 		= 	./zlib
SRC_HDR		= 	./src/



# Compiler flags
CFLAGS =  $(DEVEL) $(DEBUG_OPTION) -D$(CUSTOMER_OPTION) -D$(SSL_OPTION) -D$(MACHINE) -D$(PROGRAM) -D$(COMPANY) \
		  -L$(Z_LIB) -I$(GLOBAL_HDR)  -I$(TRANS_HDR) -I$(REG_HDR) -I$(Z_HDR) -I$(SRC_HDR) \
		  $(SQL_INCLUDE) $(LDAP_INCLUDE) $(SSL_INCLUDE) \
		  -D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64 $(EXTERNAL_TRANSCEND_ENV_CFLAGS)




OBJECT = check_reg.o reg_query.o customer_set.o dlt_time_calc.o entry.o email.o external_transweb.o \
		 find_email.o find_reg_request.o fss.o init_trans_socket.o mask_filelist_check.o \
		 get_big_file.o info_big_file.o lookup_release_keyword.o keyword.o open_big_file.o owner_dlt.o process_request.o put_big_file.o \
		 rtv_user_email.o  sql_lib.o send_secure_keyword.o send_rtv_email.o send_file.o time_stamp.o

OBJECT	:= 	$(addprefix $(DIR), $(OBJECT))

include generic.def


# Mazieri's LDAP
OBJECT += ./src/ldapfsl.o src/ldapfsl_trans.o

OUTPUT = bin/icom.int
# include(s) below will set the OUTPUT according to some variables
include output_debug.def


$(info BEGIN)
$(info $(OBJECT))
$(info END)


ssl64icomd:  $(OBJECT)
	@echo
	@echo ":: ssl64make_server.mk --target='ssl64icomd' --output='$(OUTPUT)' (Internal Intercom Server)"
	@echo
	$(CC) $(CFLAGS) $(OBJECT) \
	./shared_intd_lib.a \
	./free_server_lib.a \
	./shared_intd_lib.a \
	./free_reg_lib.a \
	./free_server_lib.a \
	./shared_intd_lib.a \
	$(SQL_LIB) \
    $(LDAP_LIBS) \
    $(SSL_LIBS) \
    zlib/libz.a \
    -lm -ldl -o $(OUTPUT)

.DEFAULT: ssl64icomd
