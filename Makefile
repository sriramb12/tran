# This makefile organize and automates Transcend/Intercom/transFTP/Transweb build


# Set our own default goal to list options available
#.DEFAULT_GOAL := list

# GENERAL COMMANDS
RM 		 = rm -f
MKDIR 	 = mkdir -p -v
RMDIR 	 = rm -rf --verbose
FIND 	 = find . -type f -name
CTAGS	 = /run/pkg/TWW-ctags-/5.7/bin/ctags
GROFF	 = /usr/bin/nroff
mkserver    = mk/serv
mkclient    = mk/client
mktrans    = mk/trans

########### HELP ##################
.PHONY: list
list:
	@echo ":: List of Make targets available"
	@echo
	@egrep -e '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m    %s\n", $$1, $$2}'
	@echo

# GENERAL RULES
.clean-build:
	$(FIND) "*.o" -exec $(RM) {} \;
	$(RM) -R ./docs/BUILD

.clean-archive:
	$(RM) lib/*.a

.clean-executable:
	$(FIND) "*.debug" -exec $(RM) {} \;
	$(RM) icom free.icomd ssl.icomd obsolete.ssl.intd ssl.tftpd ssl.transcend trans obsolete.transweb.bin obsolete.transweb.server ldap_info ldap_login obsolete.foundry_send
	$(RM) *.a

clean: .clean-build .clean-archive

clean-all: .clean-build .clean-executable .clean-archive

bin:
	mkdir -p bin

########### CTAGS GEN #############
build-tags: ## Build CTags locally
	$(RM) ./tags
	$(CTAGS) -V -R -a --tag-relative -f ./tags *


########### MANPAGES GEN #############
build-docs:  ## Build manpages and HTML
	@echo ":: Building manpages"
	$(RM) docs/trans.a docs/trans.html
	$(RM) docs/transrc.a docs/transrc.html
	$(GROFF) -e -mandoc docs/trans.1 > docs/trans.a
	$(GROFF) -e -mandoc docs/transrc.1 > docs/transrc.a
	cat docs/trans.1 | groff -mandoc -Thtml &> docs/trans.html
	cat docs/transrc.1 | groff -mandoc -Thtml &> docs/transrc.html

########### STATIC WEBSITE GEN #############
build-site:  build-docs ## Build Transcend static website
	@echo ":: Building static website"
	$(RM) docs/BUILD/*
	./scripts/gen_site.py

zlib/zlib.h:
	cd zlib && tar -xvzf zlib-1.2.3.tgz ; cd ..

########### RENEE ZLIB ############
build-zlib: zlib/zlib.h
	cd ./zlib; 


############ TRANSCEND ############
build-transcend-server: 
	$(MAKE) -f $(mktrans)/translib.mk  SERVER=1
	$(MAKE) -f $(mktrans)/serverlib.mk SERVER=1
	$(MAKE) -f $(mktrans)/reglib.mk    SERVER=1
	$(MAKE) -f $(mktrans)/server.mk    SERVER=1

build-transcend-client: ## Build Transcend Client
	$(MAKE) -f $(mktrans)/client.mk

build-transcend-full: build-transcend-server clean build-transcend-client ## Build Transcend Client and Server in one shot


########### TRANSWEB ############
build-transweb-client-icom-lib: .clean-build .clean-archive ## Build Intercom.so for Perl interface in TransWeb page
	$(MAKE) -f transweb_external_client_library.mk Intercom.so


############ TRANSFTP ############  libopenssl.def will set USE_OS_SSL=1 for all builds BUT RH6
build-transftp-server: 
	$(MAKE) -f ssl64transftpmake_reglib.mk    SERVER=1  USE_OS_LDAP=1 USE_OS_MYSQL=1 
	$(MAKE) -f ssl64transftpmake_serverlib.mk SERVER=1  USE_OS_LDAP=1 USE_OS_MYSQL=1
	$(MAKE) -f ssl64transftpmake_translib.mk  SERVER=1  USE_OS_LDAP=1 USE_OS_MYSQL=1 
	$(MAKE) -f ssl64transftpmake_server.mk    SERVER=1  USE_OS_LDAP=1 USE_OS_MYSQL=1


############ INTERCOM ############
# External
build-intercom-external-server: bin
	$(MAKE) -f $(mkserver)/ext/reglib.mk     SERVER=1
	$(MAKE) -f $(mkserver)/ext/serverlib.mk  SERVER=1
	$(MAKE) -f $(mkserver)/ext/translib.mk   SERVER=1
	$(MAKE) -f $(mkserver)/ext/server.mk     SERVER=1

build-intercom-external-client: bin ## Build Intercom External Client
	$(MAKE) -f $(mkclient)/ext/client.mk

build-intercom-external-full: build-intercom-external-server clean build-intercom-external-client ## Build Intercom External Client and Server in one shot

# Internal
build-intercom-internal-server: bin
	$(MAKE) -f $(mkserver)/int/serverlib.mk SERVER=1
	$(MAKE) -f $(mkserver)/int/reglib.mk    SERVER=1
	$(MAKE) -f $(mkserver)/int/translib.mk  SERVER=1
	$(MAKE) -f $(mkserver)/int/server.mk    SERVER=1

build-intercom-internal-client: bin ## Build Intercom Internal Client
	$(MAKE) -f $(mkclient)/int/client.mk

build-intercom-internal-full: build-intercom-internal-server clean build-intercom-internal-client ## Build Intercom Internal Client and Server in one shot

