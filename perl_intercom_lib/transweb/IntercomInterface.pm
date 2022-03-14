package IntercomInterface;
use strict;

use Cwd;
use Intercom;
use Data::Dumper;

require "LogOff.pm";


my $SERVER_HOST = "intercom.freescale.com";
my $SERVER_ADDR = "192.88.157.190";
my $SERVER_PORT = 443;


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

Log::cgiParamameters();

sub new {
  Log::startFunction();
  my ($class,$uid,$name,$friendly_email,$ip_addr) = @_;
  Log::var("\$uid=$uid, \$name=$name, \$friendly_email=$friendly_email ,\$ip_addr=$ip_addr");
  my $self = {};
  bless $self, $class;
  $self->init($uid,$name,$friendly_email,$ip_addr);
  Log::endFunction();
  return $self;
}

sub init {
  Log::startFunction();
  my ($self,$uid,$name,$friendly_email,$ip_addr) = @_;
  $uid = $uid || "";
  $name = $name || "";
  $friendly_email = $friendly_email || "";
  # we must call defaults_init before using the library
  Intercom::defaults_init($ip_addr,$uid,$friendly_email,$name);
  $self->_set_uid($uid);
  $self->_set_name($name);
  $self->_set_friendly_email($friendly_email);
  Log::endFunction();
}

sub _set_uid {
  Log::startFunction();
  my ($self,$uid) = @_;
  $self->{UID} = lc($uid);
  Log::endFunction();
}

sub get_uid { return $_[0]->{UID}}

sub _set_name {
  Log::startFunction();
  my ($self, $name) = @_;
  $self->{NAME} = $name;
  Log::endFunction();
}

sub get_name { return $_[0]->{NAME}}

sub _set_friendly_email {
  Log::startFunction();
  my ($self, $email) = @_;
  $self->{FRIENDLY_EMAIL} = $email;
  Log::endFunction();
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
  Log::startFunction();
  # NOTICE! This is a class method not an INSTANCE METHOD
  # Call it like this,
  # my $name = IntercomInterface::authenticate($uid,$pwd);
  # there is no $self-> there
  my ($uid,$pwd,$ip_addr) = @_;
  return (undef,"") unless ($uid && $pwd);
  Intercom::defaults_init($ip_addr,$uid,'FRIENDLY_EMAIL','NAME'); # must call this first
  Log::var("uid=$uid, pwd=$pwd");
  my ($ret) = Intercom::icom_authen($uid,$pwd);
  Log::var("ret=$ret");
  my $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");

  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    my ($friendly_email,$user_name) = split /;/, $msg;

    # $msg will contain the User's Friendly Email and Name, eg Dusty Jones
    # dusty.jones@motorola.com;Dusty Jones
    Log::endFunction();
    return ($user_name,$friendly_email);
  }
  Log::endFunction();
  return (undef, $msg,undef);
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
  Log::startFunction();
  Log::var("pwd", getcwd());
  my ($companies) = Intercom::icom_company_rtv();
  Log::var("\$companies=", Dumper($companies));
  my $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");
  unless (@$companies) {
    # no companies were returned, or @$companies is null
    Log::endFunction();
    return (undef,$msg);
  }
  # we have an array of company information, lets parse it
  my $company_info;
  foreach my $info (@$companies) {
    my ($friendly_name) = $info =~ /^([\w\)\( ]+);/;
    $company_info->{$friendly_name} = $info;
  }
  Log::endFunction();
  return $company_info;
}

=pod

@$keys = get_user_keys();
Get the keys associated with files that this user has placed on intercom

Return value is an array_ref, undefined on error

=cut

sub get_user_keys {
  Log::startFunction();
  my ($self,$repository) = @_;
  $repository = $repository || REPOSITORY_ALL;
  my ($keys) = Intercom::icom_user_keys_rtv($repository);
  my $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  unless ($keys) {
    # no keys were returned, or @$keys is null
    Log::endFunction();
    return (undef,$msg);
  }
  Log::endFunction();
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
  Log::startFunction();
  my ($self,$repository) = @_;
  $repository = $repository || REPOSITORY_ALL;
  my ($keys) = Intercom::icom_secure_keys_rtv($repository);
  my $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  unless ($keys) {
    # no keys were returned or @$keys is null
    Log::endFunction();
    return (undef,$msg);
  }
  Log::endFunction();
  # we have keys return them without further delay
  return $keys;
}

=pod

\%repositories = get_repositories
Get the repositories that are available to store files

Return value is an hash_ref, undefined on error

=cut

sub get_repositories {
  Log::startFunction();
  my ($self) = @_;

#   my ($repositories) = Intercom::icom_repository_rtv();
#   my $msg = Intercom::get_global_msg();

  my @repos = ("External;TransWeb;e;E;$SERVER_HOST;$SERVER_ADDR;$SERVER_PORT");
  my $repositories = \@repos;
  my $msg = "List of repositories";
  Log::var("\$repositories=", Dumper($repositories));
  Log::var("msg=$msg");
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  unless (@$repositories) {
    # no repositories were returned or @$repositories is null
    Log::endFunction();
    return (undef,$msg);
  }
  # we have some repositories
  Log::endFunction();
  return $repositories;
}


=pod

\%companies = get_companies
Get the repositories that are available to store files

Return value is an hash_ref, undefined on error

=cut

sub get_companies {
  Log::startFunction();
  my ($self) = @_;

 ## see $devel 
  my @companies = ("NXP Semiconductors;$SERVER_HOST;$SERVER_ADDR;$SERVER_PORT;$self->{FRIENDLY_EMAIL};Transweb dir;");
  
  Log::var("companies", Dumper(\@companies));
  Log::endFunction();
  return \@companies;


  # ORIG: 

#   my ($companies) = Intercom::icom_company_rtv();
#   Log::var("companies=", Dumper($companies));
#   my $msg = Intercom::get_global_msg();
#   Log::var("msg=$msg");
#   unless (@$companies) {
#     # no companies were returned or @$companies is null
#     return (undef,$msg);
#   }
#   Log::var("companies", Dumper($companies));
#   # we have some companies
#   Log::endFunction();
#  return $companies;
}

=pod

\%status = get_status($primary_keyword,$secondary_keyword)
Get the status flags for a given keyword
Return values is a hash_ref, undefined on error

=cut

sub get_status {
  Log::startFunction();
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  Log::var("calling Intercom::icom_status($primary_keyword, $secondary_keyword)");
  return (undef,"No Keyword") unless $primary_keyword;
  $secondary_keyword = $secondary_keyword || 0;
  my ($status_array_ref)     = Intercom::icom_status($primary_keyword,
						     $secondary_keyword);
  my $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");
  unless ($status_array_ref) {
    # no status was returned
    Log::endFunction();
    return (undef,$msg);
  }
  # we have status
  my $array_string = Intercom::icom_c_array_2_perl_csv_string($status_array_ref);
  if (!$array_string)
  {
      Log::endFunction();
      return (undef,"bad array from keyword status");
  }
  my @array_status = split(';', $array_string);
  Log::var("array_string=", Dumper(\@array_status));
  my $status = {
		GLOBAL_EMAIL_SET  => ($array_status[GLOBAL_EMAIL_SET] == DBYES) ? 'YES' : 'NO',
                FILE_EMAIL_SET    => ($array_status[FILE_EMAIL_SET] == DBYES) ? 'YES' : 'NO',
	        RETRIEVAL_LOCK_SET=> ($array_status[RETRIEVAL_LOCK_SET] == DBYES) ? 'YES' : 'NO',
	        DELETE_SET        => ($array_status[DELETE_SET] == DBYES) ? 'YES' : 'NO',
	        DAYS_EXTENDED     => $array_status[DAYS_EXTENDED],
	        OWN_DATA          => ($array_status[OWN_DATA] == DBYES) ? 'YES' : 'NO',
	        TRANSFER_COMPLETE => ($array_status[TRANSFER_COMPLETE] == DBYES) ? 'YES' : 'NO',
	        #END_STATUS        => ($array_status[END_STATUS] == DBYES) ? 'YES' : 'NO',
	       };
  Log::endFunction();
  return $status;
}

=pod

$info = get_info($primary_keyword,$secondary_keyword)
Returns info about the given keyword

=cut

sub get_info {
  Log::startFunction();
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my $ret = Intercom::icom_info($primary_keyword,$secondary_keyword);
  my $msg = Intercom::get_global_msg();
  Log::var("ret=$ret msg=$msg");
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($msg) {
    Log::endFunction();
    return $msg;
  }
  Log::endFunction();
  return undef;
}

=pod

$history = get_history($primary_keyword,$secondary_keyword)
Returns the history of the keyword

=cut

sub get_history {
  Log::startFunction();
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my $ret = Intercom::icom_history($primary_keyword,$secondary_keyword);
  my $msg = Intercom::get_global_msg();
  Log::var("ret=$ret msg=$msg");
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($msg) {
    my @lines = split /\n/, $msg;
    Log::endFunction();
    return \@lines
  }
  Log::endFunction();
  return undef;
}

=pod

$size = get_size($primary_keyword,$secondary_keyword)
Returns the size of the file for the keyword

=cut

sub get_size {
  Log::startFunction();
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my $ret = Intercom::icom_size($primary_keyword,$secondary_keyword);
  my $msg = Intercom::get_global_msg();
  Log::var("ret=$ret msg=$msg");
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($msg) {
    Log::endFunction();
    return $msg;
  }
  Log::endFunction();
  return undef;
}

=pod

$filename = get_init($primary_keyword,$secondary_keyword, $retrieve_compressed, $delete)
Makes the connection with the intercom server so it
knows that we want to make a file transfer of the
keyword.

=cut

sub get_init {
  Log::startFunction();
  my ($self, $primary_keyword, $secondary_keyword, $retrieve_compressed, $delete) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  $delete = $delete || DBNO; # default to no, we won't delete the file

  my ($filename, $size, $size_comp, $comp_mode);
  
 ## Intercom::icom_get_init() brings wrong $size_comp
  my $msg = $self->get_size($primary_keyword, $secondary_keyword);
  Log::var("msg=$msg");
  if ($msg)
  {
     #expected:
     #File:        TESTING.txt
     #Original size:    139 bytes
     #Compressed size:  89 bytes
     if ( $msg =~ m/Original\s+size:\s+(\d+)/ )
     {
        $size      = $1;      
     }
     if ( $msg =~ m/Compressed\s+size:\s+(\d+)/ )
     {
        $size_comp      = $1;      
     }
  }
  
  my ($ret) = Intercom::icom_get_init($primary_keyword, $secondary_keyword, $delete);
  $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;

  # error during get_init?
  if ($ret != SUCCESS) {
    warn 'ERROR DURING INTERCOM GET INIT' if $DEBUG;
    Log::endFunction();
    return (undef,$msg);
  }

  # file_name, size_reg size_comp, file_type, and comp_mode
  # %s         %d       %d         %c             %d

  # $msg should look something like
  # filename SIZE SIZE char char
  # file.tgz 479624 16895 e 1
  my ($filename, $comp_mode) =
    $msg =~ /^(.{1,}) \d{1,} \d{1,} \w{1} (\d{1,})/;
  
  Log::var("\$filename=$filename, \$size=$size, \$size_comp=$size_comp, \$comp_mode=$comp_mode");
  
  # signal that we are ready
  $ret = Intercom::put_short($ret);
  
  # error during the signalling?
  if ($ret != SUCCESS) {
    warn 'ERROR SIGNALING THAT CLIENT WAS READY' if $DEBUG;
    Log::endFunction();
    return (undef,$msg);
  }
 
  my $file_ptr;
  if ($comp_mode == DBYES && $retrieve_compressed == DBNO)
  { # file is compressed
    ($file_ptr) = Intercom::icom_get_compress_init($size);
    $msg = Intercom::get_global_msg();
    Log::var("msg=$msg");

    # error during compression get init?
    if (!$file_ptr) {
      warn 'ERROR DURING COMPRESSED FILE GET INIT' if $DEBUG;
      Log::endFunction();
      return (undef,$msg);
    }
  }
  Log::endFunction();
  return ($filename,$comp_mode, $file_ptr, $size, $size_comp);
}
  
sub get_close {
  Log::startFunction();
  my ($self) = @_;
  my ($ret) = Intercom::icom_get_close();
  my $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret != SUCCESS) {
    Log::endFunction();
    return (undef,$msg);
  }
  # close succeeded
  Log::endFunction();
  return 1;
}

# get ready for file transfer
sub put_init {
   Log::startFunction();
   my ($self,$file_params) = @_;
  # DBYES is 1, DBNO is 2
  my $delete = ($file_params->{DELETE_AFTER_RETRIVAL_FLAG} eq 'YES') ? DBYES: DBNO;
   
   # we need all these params to go on
   unless ($file_params->{FILE_NAME} &&
	   $file_params->{COMPANY} && 
	   $file_params->{FILE_DESCRIPTION} &&
	   $delete) {
     Log::endFunction();
     return (undef,"Don't have all of the parameters to put a file\n" . 
       " need file_name, company, and file_description");

   }
   
   Log::var("\$file_params->{FILE_NAME}=$file_params->{FILE_NAME}");
   Log::var("\$file_params->{COMPANY}=$file_params->{COMPANY}");
   Log::var("\$file_params->{FILE_DESCRIPTION}=$file_params->{FILE_DESCRIPTION}");
   Log::var("\$file_params->{ALTERNATE_EMAIL_ADDRESSES}=$file_params->{ALTERNATE_EMAIL_ADDRESSES}");
   Log::var("\$delete=$delete");
  
  my ($ret)      = Intercom::icom_put_init(
					   $file_params->{FILE_NAME},
					   $file_params->{COMPANY},
					   $file_params->{FILE_DESCRIPTION},
					   $file_params->{ALTERNATE_EMAIL_ADDRESSES},
					   $delete,DBNO);
  my $msg = Intercom::get_global_msg();
  Log::var("\$ret=$ret, \$msg=$msg");
  
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    my ($keyword) = $msg =~ /([a-zA-Z]{4}\d{4}[a-zA-Z]{1})/;
    Log::endFunction();
    return $keyword;
  }
  Log::endFunction();
  return (undef,$msg);
}

# compressed file tranfer initilization
sub put_compressed_init {
  Log::startFunction();
  my ($self,$file_params) = @_;

  # DBYES is 1, DBNO is 2
  my $delete = ($file_params->{DELETE_AFTER_RETRIEVAL_FLAG} eq 'YES') ? DBYES : DBNO;

   Log::var("file_params=", Dumper($file_params));
   Log::var("\$file_params->{FILE_NAME}=$file_params->{FILE_NAME}");
   Log::var("\$file_params->{COMPANY}=$file_params->{COMPANY}");
   Log::var("\$file_params->{FILE_DESCRIPTION}=$file_params->{FILE_DESCRIPTION}");
   Log::var("\$file_params->{ALTERNATE_EMAIL_ADDRESSES}=$file_params->{ALTERNATE_EMAIL_ADDRESSES}");
   Log::var("\$delete=$delete");
   
   my $msg =  undef;
  # filename,companyline,description,remote_email,delete
  my ($ret) = Intercom::icom_put_compress_init(
		      $file_params->{FILE_NAME},
		      $file_params->{COMPANY},
		      $file_params->{FILE_DESCRIPTION},
		      $file_params->{ALTERNATE_EMAIL_ADDRESSES},
                      $delete);
  Log::var("\$ret=$ret");
 
  if ($ret) { # return is a file pointer
    $msg = Intercom::get_global_msg();
    my ($keyword) = $msg =~ /([a-zA-Z]{4}\d{4}[a-zA-Z]{1})/;
    Log::var("keyword=$keyword msg=$msg");
    Log::endFunction();
    return ($keyword,$ret);
  }
  Log::endFunction();
  return (undef,$msg);
}
 
#close the transfer
sub put_close {
  Log::startFunction();
  my ($self,$size) = @_;
  my ($ret) = Intercom::icom_put_close($size);
  my $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");
  if ($ret == SUCCESS) {
    Log::endFunction();
    return (1,$msg);
  }
  Log::endFunction();
  return (undef,$msg);
}

# close the transfer compressed
sub put_compressed_close {
  Log::startFunction();
  my ($self,$file_pointer,$file_desc) = @_;
  my ($ret) = Intercom::icom_put_compress_close($file_pointer,$file_desc);
  my $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");
  if ($ret == SUCCESS) {
    Log::endFunction();
    return (1,$msg);
  }
  Log::endFunction();
  return (undef,$msg);
}

# close the transfer compressed
sub get_compressed_close {
  Log::startFunction();
  my ($self,$file_pointer) = @_;
  my ($ret) = Intercom::icom_get_compress_close($file_pointer);
  my $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");
  if ($ret == SUCCESS) {
  Log::endFunction();
    return 1;
  }
  Log::endFunction();
  return (undef,$msg);
}

sub email_keyword {
  Log::startFunction();
  my ($self,$primary_keyword,$secondary_keyword,
      $email_addresses,$description) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my ($ret)     = Intercom::icom_mail_keyword($primary_keyword,
					       $secondary_keyword,
					       $email_addresses,
		  			       $description);
  my $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    Log::endFunction();
    return $msg;
  }
  Log::endFunction();
  return (undef,$msg);
}

sub extend_keyword {
  Log::startFunction();
  my ($self,$primary_keyword,$secondary_keyword,$days) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my ($ret)      = Intercom::icom_extend($primary_keyword,
					 $secondary_keyword,
					 $days);
  my $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    Log::endFunction();
    return $msg;
  }
  Log::endFunction();
  return (undef,$msg);
}

sub delete_keyword {
  Log::startFunction();
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my ($ret) = Intercom::icom_delete($primary_keyword,$secondary_keyword);
  my $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    Log::endFunction();
    return $msg;
  }
  Log::endFunction();
  return (undef,$msg);
}

sub lock_keyword {
  Log::startFunction();
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my ($ret) = Intercom::icom_lock($primary_keyword,$secondary_keyword);
  my $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    Log::endFunction();
    return $msg;
  }
  Log::endFunction();
  return (undef,$msg);
}

sub unlock_keyword {
  Log::startFunction();
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my ($ret) = Intercom::icom_unlock($primary_keyword,$secondary_keyword);
  my $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    Log::endFunction();
    return $msg;
  }
  Log::endFunction();
  return (undef,$msg);
}

sub set_file_email {
  Log::startFunction();
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my ($ret) = Intercom::icom_set_file_email($primary_keyword,$secondary_keyword);
  my $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    Log::endFunction();
    return $msg;
  }
  Log::endFunction();
  return (undef,$msg);
}

sub can_file_email {
  Log::startFunction();
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  $secondary_keyword = $secondary_keyword || 0;
  my ($ret) = Intercom::icom_can_file_email($primary_keyword,$secondary_keyword);
  my $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    Log::endFunction();
    return $msg;
  }
  Log::endFunction();
  return (undef,$msg);
}

sub set_user_email {
  Log::startFunction();
  my ($self) = @_;
  my ($ret) = Intercom::icom_set_user_email();
  my $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    Log::endFunction();
    return $msg;
  }
  Log::endFunction();
  return (undef,$msg);
}

sub can_user_email {
  Log::startFunction();
  my ($self) = @_;
  my ($ret) = Intercom::icom_can_user_email();
  my $msg = Intercom::get_global_msg();
  Log::var("msg=$msg");
  warn Data::Dumper->Dumper($msg),"\n" if $DEBUG;
  if ($ret == SUCCESS) {
    Log::endFunction();
    return $msg;
  }
  Log::endFunction();
  return (undef,$msg);
}


sub show_C_Pid
{
    Log::startFunction();
    my $msg = Intercom::get_global_msg();
    Log::endFunction();
}

1;
