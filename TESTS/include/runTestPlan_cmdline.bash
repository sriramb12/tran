# Oct 2018

#general command line options for runTestPlan

rpt_dev_clients_only=0
rpt_dev_servers_only=0


rpt_prod_clients_only=0
rpt_prod_servers_only=0

rpt_small_files_only=0
rpt_large_files_only=0

rpt_help()
{
   echo "$0  [ [--dev | --dev-clients-only | --dev-servers-only ] | --prod | [--prod-clients-only | --prod-servers-only ] ]  [ --small-files-only ]"
   echo "      --dev-clients-only     => do not run Test Plan for production clients"
   echo "      --dev-servers-only     => do not run Test Plan for production servers"
   echo "      --dev                  => --dev-clients-only and --dev-servers-only"
   echo "      --prod-clients-only    => do not run Test Plan for development clients"
   echo "      --prod-servers-only    => do not run Test Plan for development servers"
   echo "      --prod                 => --prod-clients-only and --prod-servers-only"
   echo "      --small-files-only     => only for files less than 1MB"
   echo "      --large-files-only     => only for files equal or greather than 1MB"
   exit 0
}


rpt_priv_set_dev_client()
{
   rpt_prod_clients_only=0
   rpt_dev_clients_only=1
}


rpt_priv_set_dev_server()
{
   rpt_prod_servers_only=0
   rpt_dev_servers_only=1
}


rpt_priv_set_prod_client()
{
   rpt_prod_clients_only=1
   rpt_dev_clients_only=0
}

rpt_priv_set_prod_server()
{
   rpt_prod_servers_only=1
   rpt_dev_servers_only=0
}


rpt_cmd_line_allows_to_run()
{
    if [ $rpt_dev_clients_only -eq 1 -a "$rtp_CLIENT_TYPE" != "dev" ]
    then
        echo "rpt_cmd_line_allows_to_run(): client type not 'dev'"
        return 1
    fi

    if [ $rpt_dev_servers_only -eq 1 -a "`echo $rtp_SERVER | egrep -i '^prod'`" != "" ]
    then
       echo "rpt_cmd_line_allows_to_run(): server is 'production'"
       return 1
    fi

    if [ $rpt_prod_clients_only -eq 1 -a "$rtp_CLIENT_TYPE" != "prod" ]
    then
         echo "rpt_cmd_line_allows_to_run(): client not 'production'"
         return 1
    fi

    if [ $rpt_prod_servers_only -eq 1 -a "`echo $rtp_SERVER | egrep -i '^prod'`" = "" ]
    then
        echo "rpt_cmd_line_allows_to_run(): server is  NOT 'production'"
        return 1
    fi

    if [ $rpt_small_files_only -eq 1 -o $rpt_large_files_only -eq 1 ]
    then
      OneMB=1024000
      size_file=`du --bytes $rtp_FILE | awk '{print $1}'`
      if [ $rpt_small_files_only -eq 1 -a $size_file -ge $OneMB ]
      then
          echo "rpt_cmd_line_allows_to_run(): $rtp_ORIG_FILE is large"
          return 1
      else
          if [ $rpt_large_files_only -eq 1 -a  $size_file -lt $OneMB ]
          then
               echo "rpt_cmd_line_allows_to_run(): $rtp_ORIG_FILE is small"
               return 1
          fi
      fi
    fi
    return 0
}


for opt in $*
do
    case $opt  in
       --dev-clients-only)      rpt_priv_set_dev_client;;
       --dev-servers-only)      rpt_priv_set_dev_server;;
       --dev)                   rpt_priv_set_dev_client
                                rpt_priv_set_dev_server;;
       --prod_clients_only)     rpt_priv_set_prod_client;;
       --prod-servers-only)     rpt_priv_set_prod_server;;
       --prod)                  rpt_priv_set_prod_client
                                rpt_priv_set_prod_server;;
       --small-files-only)      rpt_small_files_only=1
                                rpt_large_files_only=0;;
       --large-files-only)      rpt_large_files_only=1
                                rpt_small_files_only=0;;
       --help)                  rpt_help;;
       *)                       rpt_help;;
    esac
done