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
 es     -- External Server
 ec     -- External Client
 is     -- Internal Server
 ic     -- Internal Client

Note: The bin/ folder will have the binaries after successful completion of a target

Ex:
~/tran$ ls bin
ext.client  ext.srv  int.client  int.srv  tran.client  tran.srv

2 Run:

a. generate ssl certs

b.
     Server (external)
    ./bin/ext.srv
     Client (external)
    ./bin/int.srv

