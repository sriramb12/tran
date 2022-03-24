Data Transport Tools
-----------------


1. How to Build?
--------------


a. create the following aliases for ease: (~/.bashrc)
--------------- 
alias es="./scripts/trans_build.sh build-intercom-external-server"
alias ec="./scripts/trans_build.sh build-intercom-external-client"
alias is="./scripts/trans_build.sh build-intercom-internal-server"
alias ic="./scripts/trans_build.sh build-intercom-internal-client"
alias ts="./scripts/trans_build.sh build-transcend-server"
alias tc="./scripts/trans_build.sh build-transcend-client"
alias ft="./scripts/trans_build.sh build-transftp-server"


b. Build a specific target using any of the following :
   (From top level folder )

command --    Target 
------------------------
- es     -- External Server
- ec     -- External Client
- is     -- Internal Server
- ic     -- Internal Client

Note: The bin/ folder will have the binaries after successful completion of a target

Ex:
~/tran$ ls bin
ext.client  ext.srv  int.client  int.srv  tran.client  tran.srv

2. Run:

a. generate ssl certs

There is a script to generate a good certificate for the servers,  <root>/ssl_and_certificates/generate_certificates.sh 

 

Steps: 

Transfer <root>/scripts/ssl_and_certificates/generate_certificates.sh to the host where the certificate will be generated; 

Edit the script with your host information 

Goto  <repository>/db_admin/crts 

b.
   -  Server (external) run as sudo 
    
   sudo ./bin/ext.srv -s

   - Client (external)
   
   ./bin/ext.client

