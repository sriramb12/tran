# This makefile ssl64icommake.mk is called to produce target ssl64icomd
# of the Intercom External server called './intercom/free.icomd'


# Configuration directives
ifeq "$(TRANS_DEBUG)" "YES"
DEBUG_OPTION = -DDEBUG
endif
CUSTOMER_OPTION = DMZ
SSL_OPTION 		= SSL_ENCRYPTION
MACHINE 		= LINUX
PROGRAM 		= INTERCOM
COMPANY 		= EXTERNAL


# MySQL Include Headers
SQL_LIB 	= /run/pkg/TWW-mysql-/5.0.81/lib/libmysqlclient.a
SQL_LIB1 	= /usr/lib64/libm.a


# Include Headers
GLOBAL_HDR 		= ../global_include
DB_HDR 			= ../trans_include
REG_HDR 		= ../reg_include
SQL_HDR 		= /run/pkg/TWW-mysql-/5.0.81/include
CURL_HDR 		= /usr/include/curl


# Compiler Flags
CFLAGS	=   $(DEBUG_OPTION) -D$(CUSTOMER_OPTION) -D$(SSL_OPTION) -D$(MACHINE) -D$(PROGRAM) -D$(COMPANY) \
			-I$(GLOBAL_HDR) -I$(DB_HDR)  -I$(REG_HDR)  -I$(SQL_HDR) -I$(CURL_HDR) -c   \
			-D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64


# Compiler
ifeq "$(TRANS_DEBUG)" "YES"
CC = gcc -g -o3 -Wall 
else
CC = gcc -o3 -Wall 
endif


# Mazieri's global debug
GLOBAL_DEBUG=../global_code/global_debug.o


# Files at intercom/ dir
OBJECT = check_reg.o reg_query.o company.o customer_set.o dlt_time_calc.o entry.o email.o external_transweb.o ftp.o ftp_get.o \
		 ftp_serial.o ftp_parallel.o ftp_transweb.o find_email.o find_reg_request.o fss.o init_trans_socket.o  \
		 mask_filelist_check.o get_big_file.o info_big_file.o lookup_release_keyword.o keyword.o open_big_file.o owner_dlt.o \
		 process_request.o put_big_file.o rtv_user_email.o send2foundry.o sql_lib.o send_secure_keyword.o \
		 send_rtv_email.o send_file.o time_stamp.o


ifeq "$(TRANS_DEBUG)" "YES"
OBJECT += $(GLOBAL_DEBUG)
endif


ifeq "$(TRANS_DEBUG)" "YES"
OUTPUT = free.icomd.debug
else
OUTPUT = free.icomd
endif

ssl64icomd:  $(OBJECT)
	@echo
	@echo ":: ssl64icommake.mk --target='ssl64icomd' --output='$(OUTPUT)' (External Intercom Server)"
	@echo
	$(CC) $(OBJECT) \
	../shared_intd_lib.a \
	../free_server_lib.a \
	../shared_intd_lib.a \
	../free_reg_lib.a \
	../free_server_lib.a \
	../shared_intd_lib.a \
	$(SQL_LIB) \
	$(SQL_LIB1) \
	-L/usr/lib64 -lldap60 -lssldap60 -lssl -lcrypto \
	-lz -o $(OUTPUT) 
	cp --verbose $(OUTPUT) ../


.DEFAULT: ssl64icomd

