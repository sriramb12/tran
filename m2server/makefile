# This makefile builds the Mask Prep server

# This is for Solaris
SOCK_LIB = -lnsl -lsocket
SSL_LIBDIR = ldap/solaris/ldapcsdk-5.1.7/lib
SSL_LIBS   = /_TOOLS_/dist/OSS-openssl-/1.0.1g/sparc-sun-solaris2.8/lib/libssl.a /_TOOLS_/dist/OSS-openssl-/1.0.1g/sparc-sun-solaris2.8/lib/libcrypto.a

LDAP_DIR       = ../openldap_solaris
LDAP_LIB_PATH  = $(LDAP_DIR)/lib
LDAP_LIBS      = $(LDAP_LIB_PATH)/liblunicode.a ${LDAP_LIB_PATH}/libldap.a $(LDAP_LIB_PATH)/liblber.a

SQL_HDR = /_TOOLS_/dist/OSS-mysql-/5.1.73/sparc-sun-solaris2.8/include/mysql
SQL_LIB = /_TOOLS_/dist/OSS-mysql-/5.1.73/sparc-sun-solaris2.8/lib/mysql

GLOBAL_HDR = ../global_include
DB_HDR = ../m2include
REG_HDR = ../reg_include
MACHINE = SOLARIS

CFLAGS = $(DEBUG) -Wall -DM2 -I$(GLOBAL_HDR) -I$(DB_HDR)  -I$(REG_HDR) -I$(SQL_HDR) -I../trans_include -D$(MACHINE) -DMDS_SERVER -I.


GIT_SHA := $(shell cat ../.git_sha)

CFLAGS := $(CFLAGS) -DGIT_SHA=\"$(GIT_SHA)\"

BUILD_DATE := $(shell date +"%Y%m%d")
CFLAGS := $(CFLAGS) -DBUILD_DATE=\"$(BUILD_DATE)\"


CC = gcc

CO =

PS_OBJ = ps_insert.o ps_list_rtv.o global_sql_lib.o

PS_OBJ = ps_insert.o ps_options_update.o global_sql_lib.o

TEST_OBJ = test.o shut_down.o return.o m2server_globals.o
OBJECT = audit_data.o mask_audit.o machine_audit.o global_audit.o \
	bld_header.o \
	mars.o mask_entry.o mask_files.o m2server_globals.o \
	layers_update.o mask_read.o \
	create.o entry.o \
	get_action.o  get_rmt_ret_code.o \
	header_build.o owner.o \
	process_cap_wafer.o process_cron.o process_fab.o \
	process_machine.o process_metrics.o \
	process_off_global.o process_off_template.o \
	process_partition.o \
	process_register.o process_serial.o process_pc.o process_pc_history.o  \
	process_sql.o process_template.o  process_usergroup.o \
	process_request.o process_mask.o process_global.o server.o \
	resend.o \
	read_global_params.o refresh.o report.o rmt_server_send.o \
	put_ret_code.o return.o state_file.o shut_down.o \
	sql_lib.o subdir.o tech_entry.o template_audit.o shadow.o time_calc.o \
	turned_off_mask.o rmt_man_reg.o \
	email_send.o email_handling.o \
	../global_code/global_debug.o \
	mettracks.o ldapfsl.o ldapfsl_trans.o

ldapfsl.o: ../src/ldapfsl.c
	$(CC) -D$(MACHINE) -c -I$(LDAP_DIR)/include -o ldapfsl.o ../src/ldapfsl.c

ldapfsl_trans.o: ../src/ldapfsl_trans.c
	$(CC) -D$(MACHINE) -c -I$(LDAP_DIR)/include -I../global_include -I../trans_include -I../server_include -o ldapfsl_trans.o ../src/ldapfsl_trans.c

global_debug.o: ../global_code/global_debug.c

mdd:  $(OBJECT) ../../../libraries/db_libs/m2server_lib.a
	$(CC) $(OBJECT) \
	../m2reg_lib.a \
	../m2server_lib.a \
	$(LIBS) $(LDLIBS)   \
	$(SSL_LIBS) \
	-L$(SQL_LIB) -lmysqlclient  -lz \
-lnsl -lsocket -o mdd

local_mdd:  $(OBJECT)
	$(CC) $(OBJECT) \
	../m2reg_lib.a \
	../m2server_lib.a \
	-L$(SQL_LIB) -lmysqlclient  -lz \
	$(LDAP_LIBS) \
	$(SSL_LIBS) \
	-lnsl -lsocket -lm \
-o mdd


linux64mdd:  $(OBJECT) ../../../libraries/db_libs/m2server_lib.a
	$(CC) $(OBJECT) \
	../../../libraries/db_libs/m2reg_lib.a \
	../../../libraries/db_libs/m2server_lib.a \
	../../../libraries/whobu/whobu.a \
	libdmi.a \
	-L../../../libraries/libraries \
	-L/disks/dmi2/linux_mot/ssdt/libraries \
	-l$(SQL_LIB1) \
	$(SSL_LIBS) \
	$(SOCK_LIBS) \
-o mdd

store_mars:  store_mars_data.o sql_lib.o m2server_globals.o shut_down.o
	$(CC) store_mars_data.o sql_lib.o m2server_globals.o shut_down.o \
	../../../libraries/db_libs/m2reg_lib.a \
	../../../libraries/db_libs/mask_archive_lib.a \
	../../../libraries/db_libs/m2server_lib.a \
	../../../libraries/db_libs/libx500a.a.solaris.10 \
	../../../libraries/whobu/whobu.a \
	$(WHOBU_LIBS) \
	-L$(SQL_LIB) -lmysqlclient  -lz \
	-lnsl -lsocket -o store_mars_data

ps:  ps.o sql_lib.o m2server_globals.o shut_down.o state_file.o
	$(CC) ps.o sql_lib.o m2server_globals.o shut_down.o state_file.o \
	../../../libraries/db_libs/m2reg_lib.a \
	../../../libraries/db_libs/m2server_lib.a \
	../../../libraries/db_libs/libx500a.a.solaris.10 \
	../../../libraries/whobu/whobu.a \
	$(WHOBU_LIBS) \
	-L$(SQL_LIB) -lmysqlclient  -lz \
	-lnsl -lsocket -o ps

scan_log:  scan_log.o sql_lib.o m2server_globals.o shut_down.o
	$(CC) scan_log.o sql_lib.o m2server_globals.o shut_down.o \
	../../../libraries/db_libs/m2reg_lib.a \
	../../../libraries/db_libs/m2server_lib.a \
	-L$(SQL_LIB) -lmysqlclient  -lz \
	-lnsl -lsocket -o scan_log


parse:  parse.o ../../../libraries/db_libs/m2server_lib.a
	$(CC) parse.o m2server_globals.o \
	../../../libraries/db_libs/m2reg_lib.a \
	../../../libraries/db_libs/mask_archive_lib.a \
	../../../libraries/db_libs/m2server_lib.a \
	../../../libraries/db_libs/libx500a.a.solaris.10 \
	../../../libraries/whobu/whobu.a \
	$(WHOBU_LIBS) \
	-lnsl -lsocket -o parse

get_names:  get_names.o ../../../libraries/db_libs/m2server_lib.a
	$(CC) get_names.o m2server_globals.o \
	../../../libraries/db_libs/m2reg_lib.a \
	../../../libraries/db_libs/mask_archive_lib.a \
	../../../libraries/db_libs/m2server_lib.a \
	../../../libraries/db_libs/libx500a.a.solaris.10 \
	../../../libraries/whobu/whobu.a \
	$(WHOBU_LIBS) \
	-lnsl -lsocket -o get_names

ldap_sort:  ldap_sort.o ../../../libraries/db_libs/m2server_lib.a
	$(CC) ldap_sort.o \
	../../../libraries/db_libs/m2reg_lib.a \
	../../../libraries/db_libs/mask_archive_lib.a \
	../../../libraries/db_libs/m2server_lib.a \
	../../../libraries/db_libs/libx500a.a.solaris.10 \
	../../../libraries/whobu/whobu.a \
	$(WHOBU_LIBS) \
	-lnsl -lsocket -o ldap_sort

sector:  sector.o ../../../libraries/db_libs/m2server_lib.a
	$(CC) sector.o \
	../../../libraries/db_libs/m2reg_lib.a \
	../../../libraries/db_libs/m2server_lib.a \
	../../../libraries/db_libs/libx500a.a.solaris.10 \
	../../../libraries/whobu/whobu.a \
	$(WHOBU_LIBS) \
	-lnsl -lsocket -o sector

authen_user:  authen_user.o ../../../libraries/db_libs/m2server_lib.a
	$(CC) authen_user.o \
	../../../libraries/db_libs/m2reg_lib.a \
	../../../libraries/db_libs/mask_archive_lib.a \
	../../../libraries/db_libs/m2server_lib.a \
	../../../libraries/db_libs/libx500a.a.solaris.10 \
	../../../libraries/whobu/whobu.a \
	$(WHOBU_LIBS) \
	-lnsl -lsocket -o authen_user

build_email_list:  build_email_list.o ../../../libraries/db_libs/m2server_lib.a
	$(CC) build_email_list.o \
	../../../libraries/db_libs/m2reg_lib.a \
	../../../libraries/db_libs/mask_archive_lib.a \
	../../../libraries/db_libs/m2server_lib.a \
	../../../libraries/db_libs/libx500a.a.solaris.10 \
	../../../libraries/whobu/whobu.a \
	$(WHOBU_LIBS) \
	-lnsl -lsocket -o build_email_list

merge: merge.o
	$(CC) merge.o -o merge

mars_info:  mars.o ../../../libraries/db_libs/m2server_lib.a
	$(CC) mars.o m2server_globals.o  \
	../../../libraries/db_libs/m2reg_lib.a \
	../../../libraries/db_libs/mask_archive_lib.a \
	../../../libraries/db_libs/m2server_lib.a \
	../../../libraries/db_libs/libx500a.a.solaris.10 \
	../../../libraries/whobu/whobu.a \
	$(WHOBU_LIBS) -lnsl -lsocket -o mars_info

location:  location.o ../../../libraries/db_libs/m2server_lib.a
	$(CC) location.o \
	../../../libraries/db_libs/m2reg_lib.a \
	../../../libraries/db_libs/mask_archive_lib.a \
	../../../libraries/db_libs/m2server_lib.a \
	../../../libraries/db_libs/libx500a.a.solaris.10 \
	../../../libraries/whobu/whobu.a \
	$(WHOBU_LIBS) -lnsl -lsocket -o location1

carpool:  carpool.o ../../../libraries/db_libs/m2server_lib.a
	$(CC) carpool.o \
	../../../libraries/db_libs/m2reg_lib.a \
	../../../libraries/db_libs/mask_archive_lib.a \
	../../../libraries/db_libs/m2server_lib.a \
	../../../libraries/db_libs/libx500a.a.solaris.10 \
	../../../libraries/whobu/whobu.a \
	$(WHOBU_LIBS) -lnsl -lsocket -o carpool1

fab_flow_build:	fab_flow_build.o
	gcc fab_flow_build.o m2server_globals.o \
	../../../libraries/db_libs/m2server_lib.a \
	$(DB_LIBS) $(GUI_LIBS) $(LDLIBS) \
	-o fab_flow_build

ret_build:	ret_build.o
	gcc ret_build.o m2server_globals.o \
	../../../libraries/db_libs/m2server_lib.a \
	$(DB_LIBS) $(GUI_LIBS) $(LDLIBS) \
	-o ret_build

flow_list_build:	flow_list_build.o
	gcc flow_list_build.o m2server_globals.o \
	../../../libraries/db_libs/m2server_lib.a \
	$(DB_LIBS) $(GUI_LIBS) $(LDLIBS) \
	-o flow_list_build

diefind:	diefind.o sql_lib.o
	gcc diefind.o m2server_globals.o sql_lib.o \
	../../../libraries/db_libs/m2server_lib.a \
	-L$(SQL_LIB) -lmysqlclient  -lz \
	 $(LIBS) $(LDLIBS)  \
	-o diefind

separate_logs_bld:	separate_logs_bld.o sql_lib.o
	gcc separate_logs_bld.o sql_lib.o m2server_globals.o \
	../../../libraries/db_libs/m2server_lib.a \
	-L$(SQL_LIB) -lmysqlclient  -lz \
	-o separate_logs_bld

ed:	ed.o
	$(CC) ed.o \
	../../../libraries/db_libs/trans_server_lib.a \
	../../../libraries/db_libs/trans_reg_lib.a \
	../../../libraries/db_libs/trans_server_lib.a \
	-o edd

richard:	richard.o
	$(CC) richard.o -o richard

all:
	cd ../../libraries/enh_lib; $(CO) makefile; $(MAKE)
	$(MAKE)

#	../../../libraries/db_libs/libx500a.a.solaris.10 \
#	../../../libraries/db_libs/mask_archive_lib.a \

ps_options_update:  $(PS_OBJ)
	$(CC) $(PS_OBJ) \
	$(LIBS) $(LDLIBS)   \
	-L$(SQL_LIB) -lmysqlclient  -lz \
	-o ps_options_update

ps_list_rtv:  $(PS_OBJ)
	$(CC) $(PS_OBJ) \
	$(LIBS) $(LDLIBS)   \
	-L$(SQL_LIB) -lmysqlclient  -lz \
	-o ps_list_rtv

test:	m2server_globals.o sql_lib.o test.o test1.o
	gcc m2server_globals.o  sql_lib.o  test.o test1.o \
	../../../libraries/db_libs/m2reg_lib.a \
	../../../libraries/db_libs/m2server_lib.a \
	-L$(SQL_LIB) -lmysqlclient  -lz \
	-o test

status.check:	m2server_globals.o sql_lib.o shut_down.o checkout_status_read.o
	$(CC) m2server_globals.o  sql_lib.o  shut_down.o checkout_status_read.o \
	../../../libraries/db_libs/m2reg_lib.a \
	../../../libraries/db_libs/m2server_lib.a \
	-L$(SQL_LIB) -lmysqlclient  -lz \
	-lnsl -lsocket -o status.check

