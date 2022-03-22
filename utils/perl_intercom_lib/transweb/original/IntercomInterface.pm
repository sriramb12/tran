package IntercomInterface;
use strict;

use Intercom;
=pod

IntercomInterface provides an OO wrapper around the Intercom.pm functions.
Intercom.pm was created using SWIG to wrap around a C library. The functions
are very C like.

=cut

use constant SUCCESS => 0; # This is a C thing, I told you
use constant DBYES => 1;
use constant DBNO => 2;
# the following are c-enumerated constants from
# libintercom.a that describe the file status info array
# constants are package scoped so they aren't available
# from Intercom.pm
use constant GLOBAL_EMAIL_SET   => 0;
use constant FILE_EMAIL_SET     => 1;
use constant RETRIEVAL_LOCK_SET => 2;
use constant DELETE_SET         => 3;
use constant DAYS_EXTENDED      => 4;
use constant OWN_DATA           => 5;
use constant TRANSFER_COMPLETE  => 6;
use constant END_STATUS         => 7;
# END of FILE_STATUS

use constant REPOSITORY_ALL => 'All;place holder;0;0;0;0;0';

my $DEBUG;

sub new {
  my ($class,$uid,$name,$friendly_email,$ip_addr) = @_;
  my $self = {};
  bless $self, $class;
  $self->init($uid,$name,$friendly_email,$ip_addr);
  return $self;
}

sub init {
  my ($self,$uid,$name,$friendly_email,$ip_addr) = @_;
  $uid = $uid || "";
  $name = $name || "";
  $friendly_email = $friendly_email || "";
  # we must call defaults_init before using the library
  Intercom::defaults_init($ip_addr,$uid,$friendly_email,$name);
  $self->_set_uid($uid);
  $self->_set_name($name);
  $self->_set_friendly_email($friendly_email);
}

sub _set_uid {
  my ($self,$uid) = @_;
  $self->{UID} = lc($uid);
}

sub get_uid { return $_[0]->{UID}}

sub _set_name {
  my ($self, $name) = @_;
  $self->{NAME} = $name;
}

sub get_name { return $_[0]->{NAME}}

sub _set_friendly_email {
  my ($self, $email) = @_;
  $self->{FRIENDLY_EMAIL} = $email;
}

sub get_friendly_email { return $_[0]->{FRIENDLY_EMAIL}}


=pod

$name = sub authenticate($uid,$pwd);
Check the supplied user id and password against the corporate 
LDAP directory, this authentication scheme is currently call
MOTOROLA APPLICATIONS DIRECTORY PASSWORD
at one time it was call the One IT password

Returns the users name on success and undefined on error.

Note: This is a class method, do not call it with $self, this
should be called before calling new() to make sure new is being
created with the proper user id

=cut

sub authenticate {
  # NOTICE! This is a class method not an INSTANCE METHOD
  # Call it like this,
  # my $name = IntercomInterface::authenticate($uid,$pwd);
  # there is no $self-> there
  my ($uid,$pwd,$ip_addr) = @_;
  return (undef,"") unless ($uid && $pwd);
  Intercom::defaults_init($ip_addr,$uid,'FRIENDLY_EMAIL','NAME'); # must call this first
  my ($ret,$msg) = Intercom::icom_authen($uid,$pwd);

  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    my ($friendly_email,$user_name) = split /;/, $$msg[0];

    # $msg will contain the User's Friendly Email and Name, eg Dusty Jones
    # dusty.jones@motorola.com;Dusty Jones
    return ($user_name,$friendly_email);
  }
  return (undef,$$msg[0]);
}

=pod

@%companies = get_companies();
Get the available companies from the icom server, the company strings are 
look like:

'Motorola (Austin TX);aus;dropbox3;216.27.248.55;6923;Tempe;0;no;',
'Motorola (WROC);wac;waccdb;10.64.2.119;6923;Tempe;0;no;',
'Motorola (Munich);mun;dropbox2;10.170.73.43;6923;Tempe;0;no;',
'Motorola (Local);loc;angel;172.16.7.179;6923;Tempe;0;no;',
'Dainippon (DNP);dnp;waccdb;10.64.2.119;6927;WROC;0;yes;'

Where the first element is a name, the second is a location,
I don't know what the other elements are as I haven't needed them.

We return a hash_ref that has as keys the friendly named part, 
eg. Motorola (Austin TX), and as values the entire line. For instance

'Motorola (Austin TX)'=>
    'Motorola (Austin TX)';aus;dropbox3;216.27.248.55;6923;Tempe;0;no;',

=cut

sub get_company_info {
  my ($self) = @_;
  my ($companies,$msg) = Intercom::icom_company_rtv();
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  unless (@$companies) {
    # no companies were returned, or @$companies is null
    return (undef,$$msg[0]);
  }
  # we have an array of company information, lets parse it
  my $company_info;
  foreach my $info (@$companies) {
    my ($friendly_name) = $info =~ /^([\w\)\( ]+);/;
    $company_info->{$friendly_name} = $info;
  }

  return $company_info;
}

=pod

@$keys = get_user_keys();
Get the keys associated with files that this user has placed on intercom

Return value is an array_ref, undefined on error

=cut

sub get_user_keys {
  my ($self,$repository) = @_;
  $repository = $repository || REPOSITORY_ALL;
  my ($keys,$msg) = Intercom::icom_user_keys_rtv($repository);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  unless ($keys) {
    # no keys were returned, or @$keys is null
    return (undef,$$msg[0]);
  }
  # we have keys return them without further delay
  return $keys;
}

=pod

@$keys = get_secure_keys();
Get the keys that are associated with files that this user can retrive
from intercom

Return value is an array_ref, undefined on error

=cut

sub get_secure_keys {
  my ($self,$repository) = @_;
  $repository = $repository || REPOSITORY_ALL;
  my ($keys,$msg) = Intercom::icom_secure_keys_rtv($repository);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  unless ($keys) {
    # no keys were returned or @$keys is null
    return (undef,$$msg[0]);
  }
  # we have keys return them without further delay
  return $keys;
}

=pod

\%repositories = get_repositories
Get the repositories that are available to store files

Return value is an hash_ref, undefined on error

=cut

sub get_repositories {
  my ($self) = @_;
  my ($repositories,$msg) = Intercom::icom_repository_rtv();
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  unless (@$repositories) {
    # no repositories were returned or @$repositories is null
    return (undef,$$msg[0]);
  }
  # we have some repositories
  return $repositories;
}


=pod

\%companies = get_companies
Get the repositories that are available to store files

Return value is an hash_ref, undefined on error

=cut

sub get_companies {
  my ($self) = @_;
  my ($companies,$msg) = Intercom::icom_company_rtv();
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  unless (@$companies) {
    # no companies were returned or @$companies is null
    return (undef,$$msg[0]);
  }
  # we have some companies
  return $companies;
}

=pod

\%status = get_status($primary_keyword,$secondary_keyword)
Get the status flags for a given keyword
Return values is a hash_ref, undefined on error

=cut

sub get_status {
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  return (undef,"No Keyword") unless $primary_keyword;
  $secondary_keyword = $secondary_keyword || 0;
  my ($status_array_ref,$msg) = Intercom::icom_status($primary_keyword,
						     $secondary_keyword);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  unless (@$status_array_ref) {
    # no status was returned
    return (undef,$$msg[0]);
  }
  # we have status
  my $status = {
		GLOBAL_EMAIL_SET  => ($$status_array_ref[GLOBAL_EMAIL_SET] == DBYES) ? 'YES' : 'NO',
                FILE_EMAIL_SET    => ($$status_array_ref[FILE_EMAIL_SET] == DBYES) ? 'YES' : 'NO',
	        RETRIEVAL_LOCK_SET=> ($$status_array_ref[RETRIEVAL_LOCK_SET] == DBYES) ? 'YES' : 'NO',
	        DELETE_SET        => ($$status_array_ref[DELETE_SET] == DBYES) ? 'YES' : 'NO',
	        DAYS_EXTENDED     => $$status_array_ref[DAYS_EXTENDED],
	        OWN_DATA          => ($$status_array_ref[OWN_DATA] == DBYES) ? 'YES' : 'NO',
	        TRANSFER_COMPLETE => ($$status_array_ref[TRANSFER_COMPLETE] == DBYES) ? 'YES' : 'NO',
	        #END_STATUS        => ($$status_array_ref[END_STATUS] == DBYES) ? 'YES' : 'NO',
	       };
  return $status;
}

=pod

$info = get_info($primary_keyword,$secondary_keyword)
Returns info about the given keyword

=cut

sub get_info {
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my $msg = Intercom::icom_info($primary_keyword,$secondary_keyword);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($msg) {
    return $$msg[0];
  }
  return undef;
}

=pod

$history = get_history($primary_keyword,$secondary_keyword)
Returns the history of the keyword

=cut

sub get_history {
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my $msg = Intercom::icom_history($primary_keyword,$secondary_keyword);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($msg) {
    my @lines = split /\n/, $$msg[0];
    return \@lines
  }
  return undef;
}

=pod

$size = get_size($primary_keyword,$secondary_keyword)
Returns the size of the file for the keyword

=cut

sub get_size {
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my $msg = Intercom::icom_size($primary_keyword,$secondary_keyword);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($msg) {
    return $$msg[0];
  }
  return undef;
}

=pod

$filename = get_init($primary_keyword,$secondary_keyword, $retrieve_compressed, $delete)
Makes the connection with the intercom server so it
knows that we want to make a file transfer of the
keyword.

=cut

sub get_init {
 my ($self, $primary_keyword, $secondary_keyword, $retrieve_compressed, $delete) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  $delete = $delete || DBNO; # default to no, we won't delete the file

  my ($ret, $msg) = Intercom::icom_get_init($primary_keyword, $secondary_keyword, $delete);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;

  # error during get_init?
  if ($ret != SUCCESS) {
    warn 'ERROR DURING INTERCOM GET INIT' if $DEBUG;
    return (undef,$$msg[0]);
  }

  # file_name, size_reg size_comp, file_type, and comp_mode
  # %s         %d       %d         %c             %d

  # $$msg[0] should look something like
  # filename SIZE SIZE char char
  # file.tgz 479624 16895 e 1
  my ($filename,$comp_mode) =
    $$msg[0] =~ /^(.{1,}) \d{1,} \d{1,} \w{1} (\d{1,})/;
  # signal that we are ready
  $ret = Intercom::put_short($ret);
  
  # error during the signalling?
  if ($ret != SUCCESS) {
    warn 'ERROR SIGNALING THAT CLIENT WAS READY' if $DEBUG;
    return (undef,$$msg[0]);
  }

  my $file_ptr;
  if ($comp_mode == DBYES 
  && $retrieve_compressed == DBNO)
  { # file is compressed
    ($file_ptr,$msg) = Intercom::icom_get_compress_init();

    # error during compression get init?
    if (!$file_ptr) {
      warn 'ERROR DURING COMPRESSED FILE GET INIT' if $DEBUG;
      return (undef,$$msg[0]);
    }
  }
  return ($filename,$comp_mode,$file_ptr);
}
  
sub get_close {
  my ($self) = @_;
  my ($ret,$msg) = Intercom::icom_get_close();
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret != SUCCESS) {
    return (undef,$$msg[0]);
  }
  # close succeeded
  return 1;
}

# get ready for file transfer
sub put_init {
   my ($self,$file_params) = @_;
  # DBYES is 1, DBNO is 2
  my $delete = ($file_params->{DELETE_AFTER_RETRIVAL_FLAG} eq 'YES') ? DBYES: DBNO;
   
   # we need all these params to go on
   unless ($file_params->{FILE_NAME} &&
	   $file_params->{COMPANY} && 
	   $file_params->{FILE_DESCRIPTION} &&
	   $delete) {
     return (undef,"Don't have all of the parameters to put a file\n" . 
       " need file_name, company, and file_description");

   }
  
  my ($ret,$msg) = Intercom::icom_put_init(
					   $file_params->{FILE_NAME},
					   $file_params->{COMPANY},
					   $file_params->{FILE_DESCRIPTION},
					   $file_params->{ALTERNATE_EMAIL_ADDRESSES},
					   $delete,DBNO);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    my ($keyword) = $$msg[0] =~ /([a-zA-Z]{4}\d{4}[a-zA-Z]{1})/;
    return $keyword;
  }
  return (undef,$$msg[0]);
}

# compressed file tranfer initilization
sub put_compressed_init {
  my ($self,$file_params) = @_;

  # DBYES is 1, DBNO is 2
  my $delete = ($file_params->{DELETE_AFTER_RETRIVAL_FLAG} eq 'YES') ? DBYES: DBNO;

  # filename,companyline,description,remote_email,delete
  my ($ret,$msg) = Intercom::icom_put_compress_init(
		      $file_params->{FILE_NAME},
		      $file_params->{COMPANY},
		      $file_params->{FILE_DESCRIPTION},
		      $file_params->{ALTERNATE_EMAIL_ADDRESSES},
					    $delete);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret) { # return is a file pointer
    my ($keyword) = $$msg[0] =~ /([a-zA-Z]{4}\d{4}[a-zA-Z]{1})/;
    return ($keyword,$ret);
  }
  return (undef,$$msg[0]);
}
 
#close the transfer
sub put_close {
  my ($self,$size) = @_;
  my ($ret,$msg) = Intercom::icom_put_close($size);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    return (1,$$msg[0]);
  }
  return (undef,$$msg[0]);
}

# close the transfer compressed
sub put_compressed_close {
  my ($self,$file_pointer,$file_desc) = @_;
  my ($ret,$msg) = Intercom::icom_put_compress_close($file_pointer,$file_desc);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    return (1,$$msg[0]);
  }
  return (undef,$$msg[0]);
}

# close the transfer compressed
sub get_compressed_close {
  my ($self,$file_pointer) = @_;
  my ($ret,$msg) = Intercom::icom_get_compress_close($file_pointer);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    return 1;
  }
  return (undef,$$msg[0]);
}

sub email_keyword {
  my ($self,$primary_keyword,$secondary_keyword,
      $email_addresses,$description) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my ($ret,$msg) = Intercom::icom_mail_keyword($primary_keyword,
					       $secondary_keyword,
					       $email_addresses,
					      $description);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    return $$msg[0];
  }
  return (undef,$$msg[0]);
}

sub extend_keyword {
  my ($self,$primary_keyword,$secondary_keyword,$days) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my ($ret,$msg) = Intercom::icom_extend($primary_keyword,
					 $secondary_keyword,
					 $days);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    return $$msg[0];
  }
  return (undef,$$msg[0]);
}

sub delete_keyword {
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my ($ret,$msg) = Intercom::icom_delete($primary_keyword,$secondary_keyword);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    return $$msg[0];
  }
  return (undef,$$msg[0]);
}

sub lock_keyword {
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my ($ret,$msg) = Intercom::icom_lock($primary_keyword,$secondary_keyword);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    return $$msg[0];
  }
  return (undef,$$msg[0]);
}

sub unlock_keyword {
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my ($ret,$msg) = Intercom::icom_unlock($primary_keyword,$secondary_keyword);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    return $$msg[0];
  }
  return (undef,$$msg[0]);
}

sub set_file_email {
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my ($ret,$msg) = Intercom::icom_set_file_email($primary_keyword,$secondary_keyword);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    return $$msg[0];
  }
  return (undef,$$msg[0]);
}

sub can_file_email {
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my ($ret,$msg) = Intercom::icom_can_file_email($primary_keyword,$secondary_keyword);
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    return $$msg[0];
  }
  return (undef,$$msg[0]);
}

sub set_user_email {
  my ($self) = @_;
  my ($ret,$msg) = Intercom::icom_set_user_email();
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    return $$msg[0];
  }
  return (undef,$$msg[0]);
}

sub can_user_email {
  my ($self) = @_;
  my ($ret,$msg) = Intercom::icom_can_user_email();
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    return $$msg[0];
  }
  return (undef,$$msg[0]);
}

1;
