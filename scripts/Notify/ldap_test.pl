#!/run/pkg/TWW-perl-/5.12.2/bin/perl
use 5.012;
use strict;
use warnings;

use Net::LDAP::LDIF;
use Data::Dumper;
use Net::LDAP;


my $coreid = $ARGV[0];
my $ret = undef;

## Preparing LDAP parameters
my $ldaps_url = 'ldaps://fsl-ids.freescale.net:636'; #SSL
my $base = "ou=people,ou=intranet,dc=motorola,dc=com"; #dn
my $service_dn = "cn=ds_ldap_2007,ou=application users,ou=applications,ou=intranet,dc=motorola,dc=com";
my $service_pwd = 'LDAP8mypw';

## LDAP Object instance
my $ldap = Net::LDAP->new( $ldaps_url ) or die "$@";
my $mesg = $ldap->bind( $service_dn, password => $service_pwd );

## Firstly user information
my $lmsg = $ldap->search(filter=>"(uid=$coreid)", base=>$base);
my @entries = $lmsg->entries;
#print "First attempt returned \@entries", Dumper(@entries);

my $mng_id = undef;
my @fields = qw/cn uid motFriendlyMail/;
my %dict = (
    cn => 'name',
    uid => 'coreid',
    motFriendlyMail => 'email',
);

## Mazieri's suggestion: switch to friendly email 
## lookup and hit the LDAP once more before give up the search
if ( scalar(@entries) == 0 ) {
   my $filter = "(motFriendlyMail=$coreid\@freescale.com)";
   $lmsg = $ldap->search(filter=>$filter, base=>$base);
   @entries = $lmsg->entries;
   #print "Second attempt returned \@entries", Dumper(@entries);
   my $num = scalar(@entries);
}

foreach my $entry (@entries) {
	my @attrs = $entry->attributes();
    my $employeeType = $entry->get_value("employeeType");

    ## if employeeType and it is R then
    if (($employeeType) and ($employeeType eq "R")) {
        $ret = 1;
	}
	$employeeType = undef;
}


if ( $ret ) {
    exit(0);
} else {
    exit(-666);
}
