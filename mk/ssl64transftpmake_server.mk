#
# This makefile builds the SSL TransFTP server.
#

DIR		=	intercom/


# This is for 64 bit Linux
CUSTOMER_OPTION 	= DMZ
SSL_OPTION 		= SSL_ENCRYPTION
PROGRAM 		= TRANS_FTP
COMPANY 		= NXP


GLOBAL_HDR 		= ./global_include
TRANS_HDR 		= ./trans_include
REG_HDR 		= ./reg_include
SRC_HDR			= ./src/

curdir = $(shell pwd)
LIBSSH2V :=

include libraries.def

CUR_CONFIG_LIBS := --libs

ifeq ("$(LIBSSH2V)",)
    LIBSSH2 := -lssh2
else
    LIBSSH2 := $(curdir)/lib/$(OS)/run/pkg/OSS-libssh2-/$(LIBSSH2V)/lib/libssh2.a -lgcrypt
endif

$(info libcurl 00 USE_OS_CURL=$(USE_OS_CURL))

ifeq ($(USE_OS_CURL),1)
    # Warning: OS CURL normally uses a 16k buffer (CURL_MAX_WRITE_SIZE), this would cause
    # slow SFTP transfers to foundries
    CURL_HDR        = /usr/include/curl
    CURL_LIB        = $(shell curl-config --libs)
else
    ## Build local custom curl
    CURL_BASE := /run/pkg/OSS-curl-/$(LIBCURLV)
    LOCAL_CUSTOM_CURL := $(curdir)/lib/$(OS)$(CURL_BASE)
    $(info libcurl 01 using local build LOCAL_CUSTOM_CURL = $(LOCAL_CUSTOM_CURL))
    ifeq ("$(wildcard $(LOCAL_CUSTOM_CURL)/lib/libcurl.a)","")
        ## if not already built
        $(info libcurl 02 Creating custom CURL Library, it may take a while ...)
        $(shell echo " Creating custom CURL Library it may take a while ..." > `tty`)
        ifeq ($(USE_OS_SSL),)
            CURL_SSL := $(SSL_BASE)
        endif
        nothing := $(shell $(curdir)/create_custom_curl_library_for_os.sh $(LOCAL_CUSTOM_CURL) $(LIBCURLV) $(LIBSSH2V) $(CURL_SSL))
    endif
    CURL_HDR := $(LOCAL_CUSTOM_CURL)/include/curl
    CURL_LIB := $(LOCAL_CUSTOM_CURL)/lib/libcurl.a $(LIBSSH2) -lpthread -lrt
endif

CURL_INCLUDE    = -I$(CURL_HDR)


$(info CURL_INCLUDE=$(CURL_INCLUDE) CURL_LIB=$(CURL_LIB))


# Compiler and Flags
CFLAGS = $(DEBUG_OPTION) -D$(CUSTOMER_OPTION) -D$(SSL_OPTION) -D$(MACHINE) -D$(PROGRAM) -D$(COMPANY) \
		 -I$(GLOBAL_HDR) -I$(TRANS_HDR) -I$(REG_HDR) -I$(SRC_HDR) $(CURL_INCLUDE) \
		 $(SSL_INCLUDE) $(SQL_INCLUDE) $(LDAP_INCLUDE) \
		 -D_LARGEFILE_SOURCE -U_OFF_T -D_FILE_OFFSET_BITS=64 $(EXTERNAL_TRANSCEND_ENV_CFLAGS)



OBJECT = check_reg.o reg_query.o customer_set.o dlt_time_calc.o entry.o email.o external_transweb.o ftp.o \
		 ftp_parallel.o ftp_transweb.o find_email.o find_reg_request.o fss.o init_trans_socket.o mask_filelist_check.o \
		 get_big_file.o info_big_file.o lookup_release_keyword.o keyword.o open_big_file.o owner_dlt.o process_request.o put_big_file.o  \
		 rtv_user_email.o send2foundry.o sql_lib.o send_secure_keyword.o send_rtv_email.o send_file.o time_stamp.o \


OBJECT	:= 	$(addprefix $(DIR), $(OBJECT))


include generic.def

# Mazieri's LDAP
OBJECT += ./src/ldapfsl.o src/ldapfsl_trans.o

OUTPUT = ssl.tftpd
# include(s) below will set the OUTPUT according to some variables
include output_debug.def

$(info BEGIN)
$(info $(OBJECT))
$(info END)


ssl64tftpd:  $(OBJECT)
	@echo
	@echo ":: ssl64transftpmake_server.mk --target='ssl64tftpd' --output='$(OUTPUT)' (TranFTP Server)"
	@echo
	$(CC) $(CFLAGS) $(OBJECT) \
	./shared_intd_lib.a \
	./free_server_lib.a \
	./free_reg_lib.a \
	./free_server_lib.a \
	./shared_intd_lib.a \
	./free_reg_lib.a \
	./free_server_lib.a \
    $(CURL_LIB) \
    $(SQL_LIB) \
    $(LDAP_LIBS) \
    $(SSL_LIBS) \
    zlib/libz.a \
    -lm -ldl -o ./$(OUTPUT)

.DEFAULT: ssl64tftpd
