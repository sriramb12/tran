# This makefile builds a library used by servers called shared_server_lib.
$(warning $(OS))
PROGRAM = TRANSCEND

GLOBAL_HDR = ../global_include
REG_HDR = ../reg_include
SQL_HDR = /_TOOLS_/dist/OSS-mysql-/5.1.73/sparc-sun-solaris2.8/include/mysql
SSL_HDR = /usr/local/ssl/include
TRANS_HDR = ../trans_include
COMPANY=FREESCALE

# Set up for Solaris compile
CFLAGS = $(DEBUG) -D$(SSL_OPTION) -D$(MACHINE) -D$(SWAP_OPTION) -D$(PROGRAM) -D$(COMPANY) \
	-I$(SSL_HDR) -I$(REG_HDR) -I$(GLOBAL_HDR) -I$(SQL_HDR) -I$(SSL_HDR) -I$(TRANS_HDR) -I../src -c \
		-D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64

# Use the following version of gcc on the austin network
# this also includes debug mode and printing of warnings
CC = /usr/sfw/bin/gcc   -g -Wall
# Use the following version of gcc on angel
CC = gcc

SSL_OBJ = ssl_common.o ssl_server_lib.o ssl_utils.o

OBJECT = entry.o add_line.o close_down.o check_owner.o  \
	big_disk_usage.o file.o files.o \
	dbsend_file.o dbsend_files.o disk_usage.o \
	 files_get.o files_put.o \
	dir.o   email_send.o \
	list.o lock_file.o file_append.o file_copy.o file_rename.o files_copy.o files_dlt.o \
	file_rcv.o files_list.o files_rcv.o  \
	get_random.o get_site.o  \
	line_get.o \
	mask_msg_server_send.o  \
	init_socket.o ip_addr.o server_lib.o \
	log.o names_build.o password_crypt.o password_encrypt.o \
	report_gen.o remote_email_send.o \
	send_files.o sort.o global_sql_lib.o strcpy.o   \
	time_stamp.o

# SSL Transcend server lib
trans_server_lib.a: $(OBJECT) $(SSL_OBJ)
	ar r ../trans_server_lib.a $(OBJECT) $(SSL_OBJ)
	ranlib ../trans_server_lib.a

decrypt:	crypt.o password_crypt.o
	$(CC) crypt.o password_crypt.o -o decrypt

free_server_lib.a: $(OBJECT) $(SSL_OBJ)
	ar r ../free_server_lib.a $(OBJECT) $(SSL_OBJ)
	ranlib ../free_server_lib.a

ssl_server_lib.a: $(OBJECT)$(SSL_OBJ)
	ar r ../../free_server_lib.a $(SSL_OBJ) $(OBJECT)
	ranlib ../../free_server_lib.a

m2server_lib.a: $(OBJECT)
	ar r ../m2server_lib.a $(OBJECT)
	ranlib ../m2server_lib.a

intd_server_lib.a: $(OBJECT)
	ar r ../../intd_server_lib.a $(OBJECT)
	ranlib ../../intd_server_lib.a

transweb_server_lib.a: $(OBJECT)
	ar r ../transweb_server_lib.a $(OBJECT)
	ranlib ../transweb_server_lib.a

opc_server_lib.a: $(OBJECT)
	ar r ../opc_server_lib.a $(OBJECT)
	ranlib ../opc_server_lib.a

reg_entry_add: reg_entry_add.o line_get.o # reconstruct keyword file from logs
	$(CC) reg_entry_add.o strcpy.o close_down.o line_get.o -o reg_entry_add
