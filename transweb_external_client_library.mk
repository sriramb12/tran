#
# This makefile builds the TransWeb client for RHEL6 64bits.
#

DIR	= client/


EXTRA_OPTION	=	TRANSCEND_GUI
SWAP_OPTION		=	BYTESWAP
SERVER_OPTION	=	MULTIPLE_INTERNAL_SERVERS
MISC			=	BACKUP_INTERNAL_SERVERS
COMPANY			= 	EXTERNAL_TRANSWEB
PROGRAM			=	TRANSWEB
TAR_OPTION 		= 	do_tar
FOUNDRY_OPTION	=	NONE
SSL_OPTION		=	SSL_ENCRYPTION

# Headers
TRANS_HDR 	= ./trans_include
GLOBAL_HDR 	= ./global_include
MASK_HDR 	= ./m2include
ZLIB_HDR 	= ./zlib
CFLAG_PIC       = 


SERVER=1   ##  force to use shared libraries to aboid -fPIC mismatch 


include libraries.def


# Compiler flags
CFLAGS = $(DEVEL) $(CFLAG_PIC) $(DEBUG_OPTION) -D$(EXTRA_OPTION) -D$(SWAP_OPTION) -D$(SERVER_OPTION) -D$(MISC) -D$(COMPANY) \
		 -D$(PROGRAM) -D$(TAR_OPTION) -D$(FOUNDRY_OPTION) -D$(MACHINE) -D$(SSL_OPTION) \
		 $(SSL_INCLUDE) -I$(TRANS_HDR) -I$(GLOBAL_HDR) -I$(MASK_HDR) -I$(ZLIB_HDR) $(SQL_INCLUDE) \
		 -D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64 -DLDAP_DEPRECATED=1 $(EXTERNAL_TRANSCEND_ENV_CFLAGS)



OBJECT = client_lib.o echo_stars.o email_send.o	get_ret_code.o getwd.o trans_hash.o \
		 ssl_common.o ssl_init.o ip_addr.o line_get.o list.o password_crypt.o password.o  scan.o \
		 strcpy.o time_calc.o block_get.o block_put.o block_retrieve.o block_usage.o close_down.o \
		 external_transweb.o gzio.o header.o files.o hash.o init_transweb_socket.o keyword_process_mail.o \
		 transweb_client_globals.o transweb_company.o disk_usage.o ldisk_usage.o dbget_big_file.o intercom_lib.o \
		 dbextend_big_file.o dbput_big_file.o dbput_big_files.o get_parallel_data.o put_parallel_data.o \
		 dbput_big_dir.o dbsend_request.o dbquery_user.o dbhist_big_file.o dbread_keyword.o dbsend_keyword.o \
		 get_file.o get_log_string.o get_gzip.o hash.o help_print.o is_keyword.o put_gzip.o  \
		 shared_memory_as_mapped_file.o mapped_file.o transrc_read.o trans_reg_functions.o gzio.o version_display.o user.o whereis.o ssl_client_lib.o

OBJECT := $(addprefix $(DIR), $(OBJECT))

#OBJECT += server_lib/ssl_server_lib.o server_lib/log.o

include generic.def


$(info BEGIN)
$(info $(OBJECT))
$(info END)


# BUILD
.DEFAULT: transweb

# TransWeb executable
transweb: libexternsslintercom.a ./client/transweb.o
	@echo
	@echo ":: transweb_external_client_library.mk--target='transweb' --output='$(OUTPUT)' (TransWeb)"
	@echo
	$(CC) ./client/transweb.o ./libexternsslintercom.a \
	./zlib/libz.a \
	-o ./$(OUTPUT)

# Needed for the TransWeb executable build
libexternsslintercom.a: $(OBJECT)
	@echo
	@echo ":: transweb_external_client_library.mk--target='libexternsslintercom.a' --output='libexternsslintercom.a' (TransWeb)"
	@echo
	ar r ./libexternsslintercom.a $(OBJECT)
	ranlib ./libexternsslintercom.a


##  Intercom.so library for Perl


PERL_FIND := $(shell find /usr/lib64/perl5 -name EXTERN.h)

ifeq ($(PERL_FIND),)
  PERL_FIND := $(shell find /usr/lib/perl -name EXTERN.h)
endif

PERL_INCLUDE := $(shell dirname $(PERL_FIND) )

INTERCOM_LIB_CFLAGS = -D_REENTRANT -D_GNU_SOURCE  -pipe -m64 -fPIC -I$(PERL_INCLUDE) -I/usr/include/gdbm -fno-strict-aliasing -pipe  -Wp,-D_FORTIFY_SOURCE=2  \
                      -DEXTERNAL_TRANSWEB -D$(EXTRA_OPTION) -D$(SSL_OPTION) \
                      $(CFLAGS)
                      

INTERCOM_LIB_OBJECT = perl_intercom_lib/intercom_lib_wrap.o \
                      perl_intercom_lib/fake_transweb_client.o
# client/intercom_lib.o

#client/intercom_lib.o: client/intercom_lib.c
#       gcc -c $(INTERCOM_LIB_CFLAGS) -o $@  $<

perl_intercom_lib/intercom_lib_wrap.c: trans_include/intercom_lib.h
	/bin/cp trans_include/intercom_lib.h perl_intercom_lib
	swig -perl5 -module Intercom  perl_intercom_lib/intercom_lib.h
	/bin/cp  perl_intercom_lib/intercom_lib_wrap.c  perl_intercom_lib/intercom_lib_wrap.c.orig
	echo '#include <stdio.h>'         >  perl_intercom_lib/intercom_lib_wrap.c
	echo '#include "intercom_lib.h"'  >>  perl_intercom_lib/intercom_lib_wrap.c
	cat  perl_intercom_lib/intercom_lib_wrap.c.orig >> perl_intercom_lib/intercom_lib_wrap.c

perl_intercom_lib/intercom_lib_wrap.o: perl_intercom_lib/intercom_lib_wrap.c
	gcc -c $(INTERCOM_LIB_CFLAGS)  -o $@  $<

perl_intercom_lib/fake_transweb_client.o: perl_intercom_lib/fake_transweb_client.c
	gcc -c $(INTERCOM_LIB_CFLAGS)  -o $@  $<



Intercom.so: $(INTERCOM_LIB_OBJECT)
	$(MAKE) build-zlib CFLAGS="$(CFLAGS) -fPIC"
	$(MAKE) -f transweb_external_client_library.mk CFLAG_PIC="-fPIC" libexternsslintercom.a
	/bin/mkdir -p RELEASE_DIR 2> /dev/null
	gcc -shared -Wl,--export-dynamic -fexceptions -m64 -fPIC $(INTERCOM_LIB_OBJECT) libexternsslintercom.a $(SSL_LIBS)  zlib/libz.a  -o  RELEASE_DIR/$@
	/bin/cp perl_intercom_lib/Intercom.pm RELEASE_DIR
###
#      special for Trabsweb on VMs
###
#	test -d /opt/transcend/transweb_external_work_area && /bin/cp  "RELEASE_DIR/Intercom.so"  /opt/transcend/transweb_external_work_area/lib/perl5/site_perl/5.8.5/sun4-solaris/auto/Intercom
#	test -d /opt/transcend/transweb_external_work_area && /bin/cp  "RELEASE_DIR/Intercom.pm"  /opt/transcend/transweb_external_work_area/lib/perl5/site_perl/5.8.5/sun4-solaris/Intercom.pm

