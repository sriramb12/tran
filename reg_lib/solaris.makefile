# This makefile builds a library used by the Transcend server
# called shared_reg_lib.a

GLOBAL_HDR = ../global_include
TRANS_HDR = ../trans_include
LDAP_HDR = ../m2server/ldap/solaris/ldapcsdk-5.1.7/include
REG_HDR = ../reg_include
OPEN_SSL_HDR = /usr/local/ssl/include
MACHINE = SOLARIS
EDC_OPTION = DMZ
PROGRAM = INTERCOM
SERVER_OPTION=MULTIPLE_INTERNAL_SERVERS
COMPANY=FREESCALE
SSL_OPTION = SSL_ENCRYPTION
MYSQL_HDR = /_TOOLS_/dist/OSS-mysql-/5.1.73/sparc-sun-solaris2.8/include/mysql


CFLAGS = $(DEBUG) -D$(SSL_OPTION) -D$(EDC_OPTION) -D$(SERVER_OPTION) -D$(PROGRAM) -D$(COMPANY) -D$(MACHINE) \
	-I$(LDAP_HDR) -I$(GLOBAL_HDR) -I$(TRANS_HDR) -I$(REG_HDR) -I$(OPEN_SSL_HDR) -I$(MYSQL_HDR) -c -I../src  \
		-D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64

# Use the following version of gcc on the austin network
# this also includes debug mode and printing of warnings
CC = /usr/sfw/bin/gcc   -g -Wall
# Use the following version of gcc on angel
CC = gcc

OBJECT = search_registry.o add_email_entry.o add_file_entry.o add_file_line.o add_site_entry.o \
	alarm.o \
	compare_names.o  compare_sites.o core_id_crypt.o \
	dlt_file_entry.o dlt_file_line.o dlt_site_entry.o \
	find_email.o edc_reg.o \
	init_rmt_socket.o  \
    init_ssl_socket.o \
	reg_lookup.o    rmt_server.o rmt_lib.o resend_reg.o \
	sql_reg.o strcmp1err.o strdlt_quotes.o strip_email_trailors.o \
	user_autoreg.o x500.o



free_reg_lib.a: $(OBJECT)
	ar r ../free_reg_lib.a $(OBJECT)
	ranlib ../free_reg_lib.a

# Used by MaskDesigner
m2reg_lib.a: $(OBJECT)
	ar r ../m2reg_lib.a $(OBJECT)
	ranlib ../m2reg_lib.a

# SSL Transcend library
trans_reg_lib.a: $(OBJECT)
	ar r ../trans_reg_lib.a $(OBJECT)
	ranlib ../trans_reg_lib.a

# Transweb library
transweb_reg_lib.a: $(OBJECT)
	ar r ../transweb_reg_lib.a $(OBJECT)
	ranlib ../transweb_reg_lib.a

intd_reg_lib.a: $(OBJECT)
	ar r ../../intd_reg_lib.a $(OBJECT)
	ranlib ../../intd_reg_lib.a

dfm_reg_lib.a: $(OBJECT)
	ar r ../dfm_reg_lib.a $(OBJECT)
	ranlib ../dfm_reg_lib.a

test:  test.o strcpy.o line_get.o close_down.o
	$(CC) test.o strcpy.o \
	-L/usr/lib64 -lldap60 -lssldap60 -lssl -lcrypto \
	-o test

