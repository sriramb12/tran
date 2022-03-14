#!/run/pkg/TWW-perl-/5.12.2/bin/perl
use 5.012;
use strict;
use warnings;

use Net::LDAP::LDIF;
use Data::Dumper;
use Net::LDAP;
use Net::DNS;
use POSIX;
use Carp;
use Cwd;
use DBI;


## Sometimes Intercom is not able to retrieve the right mngid.
## This script fix it. For your reference, look for the INC0355024.
## by Carlos Costa.

sub get_ldap_user {
	my $coreid = shift;

	my $results = {
            coreid => undef,
              name => undef,
             email => undef,
              role => undef,
        mng_coreid => undef,
          mng_name => undef,
         mng_email => undef
    	};

   	## PREPARING LDAP PARAMETERS ##

	## FSL LDAP
    #my $ldaps_url = 'ldaps://fsl-ids.freescale.net:636'; #SSL
    #my $base = "ou=people,ou=intranet,dc=motorola,dc=com"; #dn
    #my $service_dn = "cn=ds_ldap_2007,ou=application users,ou=applications,ou=intranet,dc=motorola,dc=com";
    #my $service_pwd = 'LDAP8mypw';

	## Maybe this is the final NXP LDAP (no managers yet)
	#my $ldap_url 	= 'ldap://US1PH84DC201V:389'; #non-SSL
	#my $base 	= 'OU=NXP,DC=wbi,DC=nxp,DC=com';
	#my $service_dn 	= 'CN=srv_rdapps01,OU=SRV Accounts,OU=Accounts,OU=Service Delivery,DC=wbi,DC=nxp,DC=com';
	#my $service_pwd = 'bU]Q_M!f_)M2WPWq';

	## The NXP LDAP where we can query safely for the manager
	my $ldap_url	= 'ldaps://inva008.eu-rdc02.nxp.com:636';
	my $service_pwd = 'Start2016!1Q';
	my $service_dn  = 'cn=trans_SA,ou=ServiceAccount,ou=services,o=NXP';
	my $base	    = 'ou=Active,ou=Identities,ou=Internal,o=NXP';


    ## LDAP Object instance
    my $ldap = Net::LDAP->new($ldap_url) or die "$@";
    my $mesg = $ldap->bind($service_dn, password => $service_pwd);


	## Firstly user information
	my $filter  = "(|(cn=$coreid)(nxpExternalID=$coreid))";
    my $lmsg    = $ldap->search(filter=>$filter, base=>$base);
    my @entries = $lmsg->entries;

    if (scalar(@entries) == 0) {
        #print "DEBUG: did not find '$coreid', filter used '$filter'\n";
		return undef;
	}

    #print "DEBUG: First LDAP attempt returned, filter used '$filter'\n";

    for my $entry (@entries) {
        my @attrs = $entry->attributes();

        $results->{name}   = $entry->get_value("fullName");
        $results->{coreid} = $entry->get_value("cn");
        $results->{email}  = $entry->get_value("mail");

	    my $mngid = $entry->get_value("manager");
	    #print "DEBUG: manager is '$mngid'\n";

	    if ($mngid =~ /cn=(\w+),ou=Active,ou=Identities,ou=Internal,o=NXP/) {
	        #print "DEBUG: MATCH! manager coreid '$mngid', '$1'\n";
	        $results->{mng_coreid} = $1;
        } else {
	        #print "DEBUG: NOMATCH! manager coreid '$mngid'\n";
            return undef;
	    }
	}

    ## Now getting manager info
	my $filter2  = "(|(cn=$results->{mng_coreid})(extensionAttribute12=$results->{mng_coreid}))";
    my $lmsg2    = $ldap->search(filter=>$filter2, base=>"$base");
    my @entries2 = $lmsg2->entries;

    if (scalar(@entries2) == 0) {
        #print "DEBUG: did not find the manager of $coreid, filter used '$filter2'\n";
		return undef;
	}

    #print "DEBUG: Second LDAP attempt returned, filter used '$filter'\n";

    for my $entry2 (@entries2) {
    	my @attrs2 = $entry2->attributes();

    	$results->{mng_name}   = $entry2->get_value("fullName");
    	$results->{mng_coreid} = $entry2->get_value("cn");
    	$results->{mng_email}  = $entry2->get_value("mail");
    }

    ## we need to adopt 'Role = User' by default
    $results->{role} = "user";

	#print "DEBUG: \$results = ".Dumper($results);

    if ($results->{mng_coreid}) {
    	return $results;
    } else {
    	return undef;
    }
}


############################################ MAIN ###############################################
my $DBH = DBI->connect("DBI:mysql:database=edc;host=dtms01-tx30", 'ra5063', 'freescale', {
    	        RaiseError => 1,
    	        PrintError => 0,
    	        PrintWarn  => 1,
    	ShowErrorStatement => 1,
    	       HandleError => \&Carp::confess,
}) or confess "Could not connect to database: $DBI::errstr";

## Load transactions that are enqueued
my $sql = "SELECT uid, coreid, mngidi, value FROM requests WHERE queue='1' AND transtype <> 'domain'";
my $sth = $DBH->prepare($sql) or confess "Could not connect prepare '$sql': $DBI::errstr";

$sth->execute() or confess "Could not execute '$sql': $DBI::errstr";

my $rows = $sth->fetchall_hashref("uid") or confess "Could not fetch all as hash ref: $DBI::errstr";

if (!defined $rows) {
    print "No rows to be processed";
	exit 666;

} else {
    foreach my $uid (keys %$rows) {
	    my $ldap_result = get_ldap_user( $rows->{$uid}->{coreid} );

	    if (!defined $ldap_result) {
	    	print "ERROR! no LDAP results for the row ".Dumper($rows->{$uid});

	    } else {
	    	if ($rows->{$uid}->{mngid} ne $ldap_result->{mng_coreid}) {

	    	    my $sql_updt = "UPDATE requests SET mngid='$ldap_result->{mng_coreid}' WHERE uid='$uid'";
	    		my $sth_updt = $DBH->prepare($sql_updt) or confess "Could not connect prepare '$sql_updt': $DBI::errstr";

	    		$sth_updt->execute() or confess "Could not execute '$sql_updt': $DBI::errstr";
	    		print "Row ".Dumper($rows->{$uid})." fixed, based on LDAP ".Dumper($ldap_result)." using ".$sql_updt;
	    	}
	    }
	}
}

## Nothing more to be processed, exiting...
exit;
