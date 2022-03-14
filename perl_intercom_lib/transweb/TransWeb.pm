package TransWeb;
use strict;
use base 'CGI::Application';
#use CGI::Session;
use Apache::Session::File;
use Template;
use PageObject;
#use LDAP;
use IntercomInterface;
use File::Basename;
use Data::Dumper;
use Date::Manip;
Date::Manip::Date_Init("TZ=EST"); # time zone


require "LogOff.pm";


# CONFIGURATION
# ENVIRONMENT

# CONFIGURE THESE IN THE CGI-SCRIPT
my $ENVIRONMENT = 'external'; # internal or external
# Temp Directory, used by CGI::Session
my $SESSION_DIRECTORY = '/tmp/transweb_cookies';
# TT Cache Directory
my $TT_CACHE_DIR = '/tmp/transweb_cache';
# COOKIE VARIABLES
my $COOKIE_NAME = 'CGISESSID_TRANSWEB';
my $COOKIE_EXPIRES = undef;
my $COOKIE_PATH = '/';
my $TEMPLATE_DIRECTORY = [];
my $ERROR_MSG;


=pod

The constants DBYES, and DBNO are used to signal YES,NO to
various libintercom routines, since they are defined as 
package globals by SWIG, they are only available in the 
Intercom.pm package, thus we redeclare them here

=cut

use constant DBYES => 1;
use constant DBNO => 2;

=pod

The maximum size of a POST able file is governed by
CGI's POST_MAX variable. Anything over 50 MB's is
probably completely unreasonable from a technical
viewpoint. Large file transfers to the Intercom
repository should use the intercom client, downloads
of any size should be ok for TransWeb

=cut

# 600 or so MBs probably unreasonable
###$CGI::POST_MAX = 1024 * 1024 * 600;

my $ERROR_MSG;

Log::cgiParamameters();

# Register the run-modes
# run_modes are called by CGI::Application->run() method
# the run method is responsible for sending the HTTP HEADERS
# it is important that run-modes never write to STDOUT or you
# will have problems. [If you write to STDOUT before the 
# HTTP HEADERS are sent, Apache will declare a Server 500, 
# premature end of script-headers error]. get_end violates
# this rule by using our overridden run() method
sub setup {
  Log::startFunction();
  my $self = shift;
  $self->start_mode('index_page');
  $self->mode_param('rm');	# parameter that contains run-mode
  
  # the run_mode names don't have to be the same as the subs,
  # I just like it better that way
  $self->run_modes(
		   'index_page'=>'index_page',     # the main page, it is only showed to
		                                   # users that have signed in
		   'sign_in'=>'sign_in',           # allow the user to sign-in this page
		                                   # is given to unknown users
		   'sign_out'=>'sign_out',  
		   'authenticate'=>'authenticate', # sign_in calls authenticate
		                                   # to process the users credentials
		   'get_start'=>'get_start',       # select a keyword to download
		   'get_end'=>'get_end',           # process the keyword and deliver the
                                                   # file to the browser
		   'repository_keywords'=>'repository_keywords',
		   'modify_file_start'=>'modify_file_start',
		   'modify_file_end'=>'modify_file_end',
		   'put_start'=>'put_start',       # the form for uploading a file
		   'put_external_start'=>'put_external_start', # like above but
                                                               # with a preselected
                                                               # repository
		   'put_mm_start'=>'put_mm_start', # For Mass Market data
		   'put_mm_external_start'=>'put_mm_external_start', # For external to Mass Market data
		   'put_end'=>'put_end',           # the actual process of uploading
		   'put_mask'=>'put_mask',       # the form for uploading a file
		   'put_mm_end'=>'put_mm_end',           # the actual process of uploading
		   'put_mm_external_end'=>'put_mm_external_end',           # the actual process of uploading
		   'read_documentation'=>'read_documentation',
		   'download_client'=>'download_client',
		   'user_look_up'=>'user_look_up',   # user directory function
		   'upload_status'=>'upload_status', # status of file upload
		   'report_start'=>'report_start',     # intro report page
		   'report_results'=>'report_results', # results from a report
		   'latest_info'=>'latest_info', # A place to keep users up
		                                 # date about activities
		  );
   Log::endFunction();
}

# ##########################################################
# run-mode subroutines
# ##########################################################

=pod

run this to set up the cgiapp

=cut

sub cgiapp_init {
  unless ( -d $TransWeb::SESSION_DIRECTORY ) {
    mkdir($TransWeb::SESSION_DIRECTORY);
  }
}

=pod

cgiapp_prerun()

run this before processing the run-mode
if the users is not signed-in take the user
to the sign-in page

=cut

sub cgiapp_prerun {
    Log::startFunction();
    my $self = shift;

    # use the CGI object to find the session id
    my $query = $self->query();

    # retrieves a session, or creates a new one if it doesn't exist
    my $session = $self->_get_current_session();
    $self->set_session($session);
    
    # check for user_id
    my $user_id = $session->{'user_id'};
    # if there is an user_id, there should also be a user_name
    my $user_name = $session->{'user_name'};
    my $friendly_email = $session->{'friendly_email'};
    
    # if there is no user_id go to sign-in page, unless of course
    # we are trying to login
    unless (($query->param('USER_ID') && $query->param('PASSWORD'))
	    || ($user_id)) {
      $self->prerun_mode('sign_in');
      return;
    }

#==========================================================
#     # ORIG: code above, this is just for test!!!!
#     my $user_id = 'b36540';
#     my $user_name = 'Sileno Rocco';
#     my $friendly_email = 'sileno.rocco@nxp.com';
#================================================================

    # at this point we have a valid user, who has authenticated
    # set up an intercom interface object
    $self->_set_intercom_interface($user_id,$user_name,$friendly_email);
    $self->_set_uid_name($user_id,$user_name,$friendly_email);
    Log::endFunction();
}

=pod

index_page()

Show the index page

=cut

sub index_page {
    my $self = shift;

    # was the user trying to go directly to a keyword download?
    my ($primary_keyword,$secondary_keyword) = $self->keyword_shortcut();

    if ($primary_keyword || $secondary_keyword) {
      # they were, go to get start for this keyword
      return $self->get_start($primary_keyword,$secondary_keyword);
    }
    # they weren't on with the index_page
    
    my $pageObject = 
      PageObject->new({TEMPLATE=>'index_page.tmpl',
		       CGI_OBJECT=>$self})
	|| die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";

    $pageObject->set('MENUS_LEFT',['menus/navigation.tmpl']);
    return $pageObject->output();
}

=pod

sign_in()

Allow the user to enter the user id and password

=cut

sub sign_in {
  my $self = shift;
  my $pageObject = 
    PageObject->new({TEMPLATE=>'sign_in.tmpl',
			       CGI_OBJECT=>$self})
	|| die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";
  $pageObject->set('MENUS_LEFT',[]);

  my ($primary_keyword,$secondary_keyword) = $self->keyword_shortcut();
  if ($primary_keyword || $secondary_keyword) {
    $pageObject->set('NEXT_STOP','get_start');
    $pageObject->set('SECURE_KEYWORD',$primary_keyword);
    $pageObject->set('FILE_KEYWORD',$secondary_keyword);
  }
  return $pageObject->output();
}

=pod

sign_out()

Process the user sign-out request

=cut

sub sign_out {
    my $self = shift;
    my $query = $self->query();
    # retrive the pre-existing session using the CGI query object
    my $session = $self->get_session();
    # delete the session from disk, taking the user id with it
    $session->{'user_name'} = undef;
    $session->{'user_id'} = undef;

    tied(%$session)->delete;
    $self->set_session(undef);
    $self->_set_uid_name(undef,undef);
    # show a successful sign_out page

    my $pageObject = 
      PageObject->new({TEMPLATE=>'sign_out_success.tmpl',
				 CGI_OBJECT=>$self})
	  || die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";
    $pageObject->set('MENUS_LEFT',[]);
    return $pageObject->output();
}

=pod

authenticate()

Check the supplied user_id and password

=cut

sub authenticate {
  Log::startFunction();
  my $self = shift;
  my $query = $self->query();
  my $user_id = lc($query->param('USER_ID'));
  my $password = $query->param('PASSWORD');
  my $ip_addr = $self->_get_ip_address();
  my ($name,$friendly_email,$message) = IntercomInterface::authenticate($user_id,$password,$ip_addr);
 # my $name='Sileno Rocco'; my $friendly_email='sileno.rocco@nxp.com'; my $message='<no error>';
 
  if ( $name ) {
    # save the user_id to the session
    my $session = $self->get_session();
    $session->{'user_id'} = $user_id;
    $session->{'user_name'} = $name;
    $session->{'friendly_email'} = $friendly_email;

    # authentication success
    # switch to index mode
    # set up the intercom interface object here 
    # because prerun didn't do it because the user wasn't known

    $self->_set_intercom_interface($user_id,$name,$friendly_email);
    $self->_set_uid_name($user_id,$name,$friendly_email);

    # see if the user was trying to go somewhere else
    # but was intercepted by the sign in requirement
    # currently this only works for get_start
    no warnings; # next stop is probably not defined
    my $next_stop = $query->param('NEXT_STOP');
    if ( $next_stop =~ /^get_start$/) {
      Log::endFunction();
      return $self->get_start();
    }
    use warnings;

    # otherwise go on with the index_page()
    Log::endFunction();
    return $self->index_page();

  } else {
    # authentication failure
    my $pageObject = 
      PageObject->new({TEMPLATE=>'sign_in.tmpl',#'authentication_failed.tmpl',
		       CGI_OBJECT=>$self,
		       ERROR_MSG=>$message,
		      })
	|| die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";


    my ($primary_keyword,$secondary_keyword) = $self->keyword_shortcut();
    if ($primary_keyword || $secondary_keyword) {
      $pageObject->set('NEXT_STOP','get_start');
      $pageObject->set('SECURE_KEYWORD',$primary_keyword);
      $pageObject->set('FILE_KEYWORD',$secondary_keyword);
    }
    $pageObject->set('MENUS_LEFT',[]);
    my $body_output =  $pageObject->output();
    Log::var("\$body_output=", Dumper($body_output));
    Log::endFunction();
    return $body_output;
  }
}

=pod

get_start([$primary_keyword,$secondary_keyword])

If the user has a keyword, show the file information
so the user can download the file, if there is no keyword
present a screen so the user can enter a keyword

keyword is optional and should only be provided when you
are overidding the rm parameter and going directly to the
get start page for a given keyword, normally we will
get it from the query object

=cut

sub get_start {
  Log::startFunction();
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  # we can pass in a keyword or we can get one from the query object
  my $query = $self->query();
  my $pageObject = 
    PageObject->new({CGI_OBJECT=>$self})
      || die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";
  
  $pageObject->set('MENUS_LEFT',['menus/navigation.tmpl']);

  # get the keyword or go to a page to enter a keyword
  $primary_keyword = $primary_keyword || $query->param('SECURE_KEYWORD');
  $secondary_keyword = $secondary_keyword || $query->param('FILE_KEYWORD');
  my $repositories;

  $pageObject->set('SECURE_KEYWORD',$primary_keyword);
  $pageObject->set('FILE_KEYWORD',$secondary_keyword);

  if (!keyword_present($primary_keyword,$secondary_keyword)) {
    $pageObject->set('RM','get_start');
    $pageObject->set('TEMPLATE','get_a_keyword.tmpl');
    $pageObject->set('REPOSITORIES',$self->get_repositories());
    Log::endFunction();
    return $pageObject->output();
  }

  my ($info,$msg) = $self->get_file_information($primary_keyword,$secondary_keyword);

  if ($info) {
    $pageObject->set('FILE',$info);

    if ($info->{DELETED} eq 'YES') { # the file has been deleted
      $pageObject->set('TEMPLATE','obsolete_keyword.tmpl');
    } else {
      $pageObject->set('TEMPLATE','get_start.tmpl');
    }
  } else {
    $pageObject->set('TEMPLATE','error.tmpl');
    $pageObject->set('ERROR_MSG',$msg);
  }
  Log::endFunction();
  return $pageObject->output();
}

=pod

repository_keywords()

This run-mode allows the user to see keywords only
for certain repositories, in the event that no
keywords exist for a certain repository, the
pageObject->output() function will redirect
to the error page

=cut

sub repository_keywords {
  my $self = shift;
  my $query = $self->query();
  my $selected_repository = $query->param('REPOSITORIES');
  my $repositories = $self->get_repositories();

  my $ii = $self->_get_intercom_interface();


  

  my $pageObject = PageObject->new({TEMPLATE=>'repository_keywords.tmpl',
		     CGI_OBJECT=>$self})
    || die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";  

  my $menu_run_mode = $query->param('MENU_RUN_MODE') || 'get_start';

  $pageObject->set('MENU_RUN_MODE',$menu_run_mode);
  $pageObject->set('MENUS_LEFT',['menus/navigation.tmpl']);


  if ($TransWeb::ENVIRONMENT eq 'internal') {
      my $user_keywords = $ii->get_user_keys($selected_repository);
      my $secure_keywords = $ii->get_secure_keys($selected_repository);
      $pageObject->set('SECURE_KEYS',$secure_keywords);
      $pageObject->set('USER_KEYS',$user_keywords);

  }


  $pageObject->set('REPOSITORIES',$repositories);

  $pageObject->set('SELECTED_REPOSITORY',_friendly_company_name($selected_repository));
  return $pageObject->output();

}

=pod

get_end()

Download the file to the browser, this involves getting the
data from the Intercom repository and printing it to STDOUT
a buffer at a time, to do this we must print directly to 
STDOUT bypassing the usual run mode

=cut

sub get_end {
  my $self = shift;
  my ($uid,$name) = $self->get_uid_name();
  my $query = $self->query();
  my $primary_keyword = $query->param('SECURE_KEYWORD');
  my $secondary_keyword = $query->param('FILE_KEYWORD') ;
  my $delete_after_retrieval_flag = $query->param('DELETE_AFTER_RETRIEVAL_FLAG') || 'NO';
  my $retrieve_compressed = $query->param('RETRIEVE_COMPRESSED') || 'NO';
  # DBYES is 1, DBNO is 2
  my $delete = ($delete_after_retrieval_flag eq 'YES') ? DBYES: DBNO;
  my $compress = ($retrieve_compressed eq 'YES') ? DBYES: DBNO;

  my $pageObject = 
    PageObject->new({CGI_OBJECT=>$self})
      || die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";

  $pageObject->set('MENUS_LEFT',['menus/navigation.tmpl']);

  $pageObject->set('SECURE_KEYWORD',$primary_keyword);
  $pageObject->set('FILE_KEYWORD',$secondary_keyword);

  if (!keyword_present($primary_keyword,$secondary_keyword)) {
    $pageObject->set('RM','get_start');
    $pageObject->set('TEMPLATE','get_a_keyword.tmpl');
    $pageObject->set('REPOSITORIES',$self->get_repositories());    
    return $pageObject->output();
  }

  my ($status, $msg) = $self->print_file_to_browser($primary_keyword, $secondary_keyword, $compress, $delete);

  if (!$status) { # no status, file didn't exist or something equally bad
    # get the file information
    $self->header_type('header'); # cgi-app will process
    my $info = $self->get_file_information($primary_keyword,
					   $secondary_keyword);
    $pageObject->set('TEMPLATE','error.tmpl');
    $pageObject->set('ERROR_MSG',$msg);
    return $pageObject->output();
  } else {
    return '';
  }
}

=pod

put_start()

Display the form used to upload a new file to the 
Intercom repository

=cut

sub put_start {
  my $self = shift;
  my $pageObject = 
    PageObject->new({TEMPLATE=>'put_start.tmpl',
			       CGI_OBJECT=>$self})
	|| die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";


  my $companies = $self->get_companies();

  $pageObject->set('MENUS_LEFT',['menus/navigation.tmpl']);
  $pageObject->set('COMPANIES',$companies);
  $pageObject->set('PID', $$);
  return $pageObject->output();
}


=pod

put_mm_start()

Display the form used to upload a new file to the 
Intercom repository

=cut

sub put_mm_start {
  my $self = shift;
  my $pageObject = 
    PageObject->new({TEMPLATE=>'put_mm_start.tmpl',
			       CGI_OBJECT=>$self})
	|| die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";


  my $companies = $self->get_companies(); # this is a hash reference
  my $mm_companies = []; # An array reference of the new companies
  foreach my $company (keys %$companies)
    {
      if ($company =~ /Mass Market Customer/) # I changed this line
	# I removed the carrot
	{
	  # we are taking the standard companies and
	  # finding just the mass market ones
	  # we will then reassign companies to 
	  # just the mass market ones
	  push @$mm_companies, $company;
	} # end of if
    } # end of foreach
  $companies = $self->get_companies($mm_companies);
  # Companies should now be reassigned to just the 
  # mass market companies

  $pageObject->set('MENUS_LEFT',['menus/navigation.tmpl']);
  $pageObject->set('COMPANIES',$companies);
  return $pageObject->output();
}

=pod

put_mm_external_start()

Display form used to upload file to Freescale Support

=cut

sub put_mm_external_start {
  my $self = shift;
  my $pageObject = 
    PageObject->new({TEMPLATE=>'put_mm_external_start.tmpl',
			       CGI_OBJECT=>$self})
	|| die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";


  my $companies = $self->get_companies();
  $pageObject->set('MENUS_LEFT',['menus/navigation.tmpl']);
  $pageObject->set('COMPANIES',$companies);
  return $pageObject->output();
}

=pod

put_external_start()

Display the form used to upload a file to the external repository.
This is just like put_start, except is has a preselected repository

=cut

sub put_external_start {
  my $self = shift;
  my $pageObject = 
    PageObject->new({TEMPLATE=>'put_external_start.tmpl',
			       CGI_OBJECT=>$self})
        || die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";


  my $companies = $self->get_companies();
  my $ext_rep;
  foreach my $company (keys %$companies) {
    if ($company =~ /^External TransWeb Customer/) {
						  $ext_rep = $company;
        					  last;
        					  }
  }

  $pageObject->set('MENUS_LEFT',['menus/navigation.tmpl']);
  $pageObject->set('COMPANY',$ext_rep);
  return $pageObject->output();
}

=pod

put_end()

Process the uploaded file, this is not suitable for large files

=cut

sub put_end {
  
  Log::startFunction();
  my $self = shift;

  # All the params are present, upload the file
  # change some session variables to watch the file go up
  
  my $session = $self->get_session();
  # initialize the session param so we can track progress
  $session->{'TRANSFER_IN_PROGRESS'} = 1;
  $session->{'BYTES_TRANSFERRED'} = 0;

  my $file_params = $self->get_file_params();

  my $still_needed = $self->verify_file_params($file_params);
  my $pageObject = 
    PageObject->new({CGI_OBJECT=>$self})
	|| die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";
  $pageObject->set('MENUS_LEFT',['menus/navigation.tmpl']);

  if (keys %$still_needed) {
    # All the params aren't present, allow user to supply the params again
    $pageObject->set('ERROR_MSG','Required fields not supplied');
    $pageObject->set('TEMPLATE','put_error.tmpl');
    $pageObject->set('STILL_NEEDED',$still_needed);
    Log::endFunction();
    return $pageObject->output();
  }

  my ($file_load_result,$error) = $self->upload_file($file_params);

  if (!$file_load_result) { # problem loading file
    # Error loading the file
    $pageObject->set('TEMPLATE','put_error.tmpl');
    $pageObject->set('ERROR_MSG',$error);
    Log::endFunction();
    return $pageObject->output();
  }

  $pageObject->set('TRANSCEND_SERVER_MESSAGE',$error);
  $pageObject->set('FILE_KEYWORD',$file_load_result->{FILE_KEYWORD});
  $pageObject->set('SIZE',$file_load_result->{SIZE});
  $pageObject->set('FILE_NAME',$file_load_result->{FILE_NAME});
  $pageObject->set('COMPANY',$file_load_result->{COMPANY});
  $pageObject->set('TEMPLATE','put_success.tmpl');

  Log::endFunction();
  return $pageObject->output();
} # End put_end

=pod

put_mm_end()

Process the uploaded file, this is not suitable for large files

=cut

sub put_mm_end {
  my $self = shift;

  # All the params are present, upload the file
  # change some session variables to watch the file go up
  
  my $session = $self->get_session();
  # initialize the session param so we can track progress

  $session->{'TRANSFER_IN_PROGRESS'} = 1;
  $session->{'BYTES_TRANSFERRED'} = 0;

  
  my $file_params = $self->get_file_params();

  my $still_needed = $self->verify_file_params($file_params);
  my $pageObject = 
    PageObject->new({CGI_OBJECT=>$self})
	|| die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";
  $pageObject->set('MENUS_LEFT',['menus/navigation.tmpl']);

  if (keys %$still_needed) {
    # All the params aren't present, allow user to supply the params again
    $pageObject->set('ERROR_MSG','Required fields not supplied');
    $pageObject->set('TEMPLATE','put_error.tmpl');
    $pageObject->set('STILL_NEEDED',$still_needed);
    return $pageObject->output();
  }

# Put service number at beginning of description
 $file_params->{FILE_DESCRIPTION} = "SR# " .  $file_params->{SERVICE_REQUEST}
   . ", " . $file_params->{FILE_DESCRIPTION}; 

  my ($file_load_result,$error) = $self->upload_file($file_params);

  if (!$file_load_result) { # problem loading file
    # Error loading the file
    $pageObject->set('TEMPLATE','put_error.tmpl');
    $pageObject->set('ERROR_MSG',$error);
    return $pageObject->output();
} # End put_mm_start

  $pageObject->set('TRANSCEND_SERVER_MESSAGE',$error);
  $pageObject->set('FILE_KEYWORD',$file_load_result->{FILE_KEYWORD});
  $pageObject->set('SIZE',$file_load_result->{SIZE});
  $pageObject->set('FILE_NAME',$file_load_result->{FILE_NAME});
  $pageObject->set('COMPANY',$file_load_result->{COMPANY});
  $pageObject->set('TEMPLATE','put_success.tmpl');

  return $pageObject->output();
} # End put_mm_end
=pod

put_mm_external_end()

Process the uploaded file, this is not suitable for large files

=cut

sub put_mm_external_end {
  my $self = shift;

  # All the params are present, upload the file
  # change some session variables to watch the file go up
  my $session = $self->get_session();
  $session->{'TRANSFER_IN_PROGRESS'} = 1;
  $session->{'BYTES_TRANSFERRED'} = 0;

  my $file_params = $self->get_file_params();
  # Send e-mail notification to Freescale Support
   $file_params->{ALTERNATE_EMAIL_ADDRESSES} = 'transbug@nxp.com';

  my $still_needed = $self->verify_file_params($file_params);
  my $pageObject = 
    PageObject->new({CGI_OBJECT=>$self})
	|| die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";
  $pageObject->set('MENUS_LEFT',['menus/navigation.tmpl']);

  if (keys %$still_needed) {
    # All the params aren't present, allow user to supply the params again
    $pageObject->set('ERROR_MSG','Required fields not supplied');
    $pageObject->set('TEMPLATE','put_error.tmpl');
    $pageObject->set('STILL_NEEDED',$still_needed);
    return $pageObject->output();
  }

# Put service number at beginning of description
 $file_params->{FILE_DESCRIPTION} = "SR# " .  $file_params->{SERVICE_REQUEST}
   . ", " . $file_params->{FILE_DESCRIPTION}; 

  my ($file_load_result,$error) = $self->upload_file($file_params);

  if (!$file_load_result) { # problem loading file
    # Error loading the file
    $pageObject->set('TEMPLATE','put_error.tmpl');
    $pageObject->set('ERROR_MSG',$error);
    return $pageObject->output();
} # End put_mm_external_end

  $pageObject->set('TRANSCEND_SERVER_MESSAGE',$error);
  $pageObject->set('FILE_KEYWORD',$file_load_result->{FILE_KEYWORD});
  $pageObject->set('SIZE',$file_load_result->{SIZE});
  $pageObject->set('FILE_NAME',$file_load_result->{FILE_NAME});
  $pageObject->set('COMPANY',$file_load_result->{COMPANY});
  $pageObject->set('TEMPLATE','put_success.tmpl');

  return $pageObject->output();
} # End put_mm_end

sub read_documentation {
  my $self = shift;
  my $query = $self->query;
  # setup for manual output
  $self->header_type('none'); # cgi-app will do nothing
  my $fh;
  open($fh,$TransWeb::DOCUMENTATION) ||
    die "No documentation file to open: $!\n";
  print $query->header(-type=> 'application/pdf',
		       -Content_Disposition => 
		       "attachment; filename=\"${TransWeb::ENVIRONMENT}_transweb_doc.pdf\"",
		      );
  while (<$fh>) {
    print;
  }
  return '';

#  my $self = shift;
#  my $pageObject = 
#    PageObject->new({TEMPLATE=>'read_documentation.tmpl',
#			       CGI_OBJECT=>$self})
#	|| die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";
#  $pageObject->set('MENUS_LEFT',['menus/navigation.tmpl']);
#  return $pageObject->output();
} # End read_documentation

sub download_client {
  my $self = shift;
  my $pageObject = 
    PageObject->new({TEMPLATE=>'download_client.tmpl',
			       CGI_OBJECT=>$self})
	|| die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";
  $pageObject->set('MENUS_LEFT',['menus/navigation.tmpl']);
  return $pageObject->output();
}

sub user_look_up {
    my $self = shift;
    my $pageObject = 
      PageObject->new({TEMPLATE=>'user_look_up_search.tmpl',
		       CGI_OBJECT=>$self})
	|| die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";
    my $query = $self->query();
    my $name = $query->param('SEARCH_USER_NAME');
    $query->param('SEARCH_USER_NAME',undef);
    
    
    if (!$name) { # no name, show the entry page
      $pageObject->set('SEARCH_USER_NAME',$name);
      $pageObject->set('TEMPLATE','user_look_up_search.tmpl');
      return $pageObject->output();
    }

    # try to lookup an address for this name
    $pageObject->set('TEMPLATE','user_look_up_results.tmpl');
    $pageObject->set('SEARCH_USER_NAME',$name);
    my $ldap = LDAP->new();
    my $user_data = $ldap->search_for_uid($name);
    $pageObject->set('USER_DATA',$user_data);
    return $pageObject->output();
}

sub modify_file_start {
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  my $pageObject =
    PageObject->new({TEMPLATE=>'modify_file_start.tmpl',
			       CGI_OBJECT=>$self})
	|| die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";
  my $query = $self->query();

  # set up the menus
  $pageObject->set('MENUS_LEFT',['menus/navigation.tmpl']);

  $primary_keyword = $primary_keyword || $query->param('SECURE_KEYWORD');
  $secondary_keyword = $secondary_keyword || $query->param('FILE_KEYWORD');

  $pageObject->set('SECURE_KEYWORD',$primary_keyword);
  $pageObject->set('FILE_KEYWORD',$secondary_keyword);

  # does a keyword exist?
  if (!keyword_present($primary_keyword,$secondary_keyword)) {
    $pageObject->set('RM','modify_file_start');
    $pageObject->set('MENU_RUN_MODE','modify_file_start');
    $pageObject->set('TEMPLATE','get_a_keyword.tmpl');
    $pageObject->set('REPOSITORIES',$self->get_repositories());
    return $pageObject->output();
  }

  my ($info,$msg) = $self->get_file_information($primary_keyword,
					$secondary_keyword);

  # does the file exist in the repository
  # or is the keyword valid?
  if (!$info) {
    # error occured
    $pageObject->set('TEMPLATE','error.tmpl');
    $pageObject->set('ERROR_MSG',$msg);
  } elsif ($info->{DELETED} eq 'YES') {
    # file was deleted
    $pageObject->set('FILE',$info);
    $pageObject->set('TEMPLATE','obsolete_keyword.tmpl');
  } elsif ($info->{OWN_DATA} ne 'YES') {
    # user is not file owner
    $pageObject->set('TEMPLATE','not_file_owner.tmpl');
  } else {
    # ok, the file should exist and the current user should be the owner
    # show the modify page
    # these are the commands shown in the link, we want to change the status
    # so we want the opposite of what is current
    if ($info->{RETRIEVAL_LOCK_SET} eq 'YES') {
      $pageObject->set('FILE_LOCK_FLAG','Unlock');
    } else {
      $pageObject->set('FILE_LOCK_FLAG','Lock');
    }
    if ($info->{FILE_EMAIL_SET} eq 'YES') {
      $pageObject->set('FILE_EMAIL_FLAG','Cancel');
    } else {
      $pageObject->set('FILE_EMAIL_FLAG','Set');
    }
    if ($info->{GLOBAL_EMAIL_SET} eq 'YES') {
      $pageObject->set('USER_EMAIL_FLAG','Cancel');
    } else {
      $pageObject->set('USER_EMAIL_FLAG','Set');
    }
    $pageObject->set('INFO',$info);
  }
  return $pageObject->output();
}


sub modify_file_end {
  my $self = shift;
  my $pageObject =
    PageObject->new({TEMPLATE=>'modify_file_start.tmpl',
			       CGI_OBJECT=>$self})
	|| die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";

  my $query = $self->query();
  my $primary_keyword = $query->param('SECURE_KEYWORD');
  my $secondary_keyword = $query->param('FILE_KEYWORD');

  $pageObject->set('SECURE_KEYWORD',$primary_keyword);
  $pageObject->set('FILE_KEYWORD',$secondary_keyword);

  # set up the menus
  $pageObject->set('MENUS_LEFT',['menus/navigation.tmpl']);

  # does a keyword exist?
  if (!keyword_present($primary_keyword,$secondary_keyword)) {
    $pageObject->set('ERROR_MSG',"No keyword to modify!\n");
    $pageObject->set('TEMPLATE','error.tmpl');
    return $pageObject->output();
  }


  if ($TransWeb::ENVIRONMENT eq 'internal') {
    if (!$primary_keyword) {
      $primary_keyword = $secondary_keyword;
      $secondary_keyword = undef;
    }
  }
  my ($info,$msg) = $self->get_file_information($primary_keyword,
						$secondary_keyword);

  # rely on server to check permissions/goodness of keywords


  my $return;
  my $message;
  my $op = $query->param('OP');
  if ($op eq 'EMAIL') {
    ($return,$message) = $self->email_keywords($primary_keyword,$secondary_keyword,
                                    $query->param('ALTERNATE_EMAIL_ADDRESSES'),
				   $query->param('DESCRIPTION'));

  } elsif ($op eq 'EXTEND') {
    my $extended_days = $query->param('EXTENDED_DAYS');
    ($return,$message) = $self->extend_keyword($primary_keyword,$secondary_keyword,
				    $extended_days);

  } elsif ($op eq 'DELETE') {
    ($return,$message) = $self->delete_keyword($primary_keyword,$secondary_keyword);

  } elsif ($op eq 'LOCK') {
    ($return,$message) = $self->toggle_keyword_lock($primary_keyword,$secondary_keyword,
					$info->{RETRIEVAL_LOCK_SET});

  } elsif ($op eq 'FILE_EMAIL') {
    ($return,$message) = $self->toggle_file_email($primary_keyword,$secondary_keyword,
				       $info->{FILE_EMAIL_SET});

  } elsif ($op eq 'USER_EMAIL') {
    ($return,$message) = $self->toggle_user_email($info->{GLOBAL_EMAIL_SET});

  } else {
    # do nothing
    ($return,$message) =  'Invalid choice, nothing modified.';
  }

  if ($return) {
    $pageObject->set('TEMPLATE','modification_indication.tmpl');
    $pageObject->set('MESSAGE',$return);
    return $pageObject->output();
  }
  # must of had a problem

  $pageObject->set('TEMPLATE','error.tmpl');
  $pageObject->set('ERROR_MSG',$message);
  # clear the error message if it exists so PageObject know we handled it ourselves
  return $pageObject->output();

}

# how many bytes have we uploaded so far?
sub upload_status {
  Log::startFunction();
  my $self = shift;
  my $pageObject =
    PageObject->new({CGI_OBJECT=>$self})
	|| die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";
  my $query = $self->query();
  # retrieves a session, or creates a new one if it doesn't exist
  my $session = $self->get_session();  

  my $in_progress = $session->{'TRANSFER_IN_PROGRESS'} || 0;
    
  Log::var("session addr = ", \$session, Dumper($session));
  
  if ($query->param('init')) {
    #warn "recieved init param\n";
    # the first time the upload status is called we want to 
    # reset the session variables to in progress
    # just in case we get here before file upload
    my $total_bytes = $session->{'BYTES_TRANSFERRED'}    || 0;
    $in_progress = 1;
    $session->{'TRANSFER_IN_PROGRESS'} = $in_progress;
    $session->{'BYTES_TRANSFERRED'}    = $total_bytes;
  }

  #  1 = transfer in progress
  #  2 = transfer is complete
  # -1 = error during transfer


  if ($in_progress == 1) {
    my $total_bytes = $session->{'BYTES_TRANSFERRED'}    || 0;
    my $bytes_trans = $self->_get_transfered_cookie($session) || 0;
    my $percent     = 0;
    my $msg = "$bytes_trans";
    if ($total_bytes > 0 && $bytes_trans > 0)
    {
        $percent = (100 * $bytes_trans) / $total_bytes;
        $msg = sprintf("(%1.f%%) %d of %d", $percent, $bytes_trans, $total_bytes);
      
    }
   
    $pageObject->set('TEMPLATE','transfer_status.tmpl');   
    Log::var("\$total_bytes=$total_bytes \$bytes_trans=$bytes_trans \$percent=$percent \$msg=$msg");
    $pageObject->set('BYTES_SAVED', $msg);
  } elsif ($in_progress == 2) {    
    $pageObject->set('TEMPLATE','transfer_complete.tmpl');
  } elsif ($in_progress == -1) {
    $pageObject->set('TEMPLATE','transfer_error.tmpl');
  } else {
    $pageObject->set('TEMPLATE','no_transfer_in_progress.tmpl');
  }
  
  if ($in_progress != 1)
  {
     $self->_remove_transfered_cookie_file($session);
  }

  Log::endFunction();
  return $pageObject->output();
}

# entry of reporting parameters
sub report_start {
  my $self = shift;
  my $pageObject =
    PageObject->new({CGI_OBJECT=>$self})
      || die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";
  $pageObject->set('TEMPLATE','reporting.tmpl');
  
  # establish defaults for these parameters
  # REPORT_KEYWORD
  $pageObject->set('REPORT_KEYWORD','');
  # START_DATE

  my $today = Date::Manip::UnixDate('today','%m/%d/%y');


  $pageObject->set('START_DATE',$today);
  # END_DATE
  $pageObject->set('END_DATE',$today);
  # REPORT_USERS
  $pageObject->set('REPORT_USERS','');
  # REPORT_PLATFORM
  $pageObject->set('REPORT_PLATFORM','Any');
  # PLATFORMS
  $pageObject->set('PLATFORMS',$self->get_report_platforms('Any'));
  # ADDRESS
  $pageObject->set('REPORT_ADDRESS','');
  # REPORT_CLIENT_VERSION
  $pageObject->set('REPORT_CLIENT_VERSION','Any');
  # CLIENT_VERSIONS
  $pageObject->set('CLIENT_VERSIONS',$self->get_report_client_versions('Any'));
  # REPORT_FROM_COMPANY
  $pageObject->set('REPORT_FROM_COMPANY','Any');
  # REPORT_TO_COMPANY
  $pageObject->set('REPORT_TO_COMPANY','Any');
  # REPOSITORIES
  $pageObject->set('COMPANIES',$self->get_report_companies('Any'));
  # REPORT_REQUEST_TYPE
  $pageObject->set('REPORT_REQUEST_TYPE','Any');
  # REQUEST_TYPES
  $pageObject->set('REQUEST_TYPES',$self->get_report_request_types('Any'));
  # REPORT_MASK_SET
  $pageObject->set('REPORT_MASK_SET','');
  # REPORT_STATUS
  $pageObject->set('REPORT_STATUS','Any');
  # STATUS
  $pageObject->set('STATUS',$self->get_report_status('Any'));

  return $pageObject->output();
}

sub report_results {}

sub latest_info {
    my $self = shift;
    
    my $pageObject = 
      PageObject->new({TEMPLATE=>'latest_info.tmpl',
		       CGI_OBJECT=>$self})
	|| die "Couldn't make page object",$PageObject::ERROR_MSG,"\n";

    $pageObject->set('MENUS_LEFT',['menus/navigation.tmpl']);

    
    return $pageObject->output();
}

sub set_session { 
  my ($self,$session) = @_;
  $self->{SESSION} = $session;
}

sub get_session { return $_[0]->{SESSION}}

# ###########################################################
# internal subroutines
# ###########################################################

# get the current session
sub _get_current_session {
  my $self = shift;
  my %session;
  my $query = $self->query();
  my $cookie = $query->cookie($TransWeb::COOKIE_NAME);
  eval {
    tie %session, 'Apache::Session::File', $cookie, {
	Directory => $TransWeb::SESSION_DIRECTORY,
        LockDirectory => $TransWeb::SESSION_DIRECTORY,
    };
  };
  if ($@) {
    $cookie = undef;
    tie %session, 'Apache::Session::File', $cookie, {
	Directory => $TransWeb::SESSION_DIRECTORY,
        LockDirectory => $TransWeb::SESSION_DIRECTORY,
    };
  }

  # put the session id into the header so the cookie will live on
  my $cookie = $self->_bake_session_cookie(\%session);
  $self->header_props(-cookie=>$cookie);

  return \%session;
}



# make a cookie that contains the session id
sub _bake_session_cookie {
  my ($self,$session) = @_;
  my $query = $self->query();
  my $sid = $session->{_session_id};
  my $cookie = $query->cookie(
			      -name=>$TransWeb::COOKIE_NAME,
			      -value=>$sid,
			      -expires=>$TransWeb::COOKIE_EXPIRES,
			      -path=>$TransWeb::COOKIE_PATH,
				);
  return $cookie;
}

sub _bake_transfered_cookie {
  Log::startFunction();
  my ($self,$transfered, $session) = @_;
  Log::var("\$transfered=$transfered");
  my $query = $self->query();
  my $cookie = $query->cookie(
			      -name=>$TransWeb::COOKIE_NAME . "_TRANSFERED",
			      -value=>$transfered,
			      -expires=>$TransWeb::COOKIE_EXPIRES,
			      -path=>$TransWeb::COOKIE_PATH,
				);
  
  my $fh = undef;
  my $filename = $self->_get_transfered_cookie_file_name($session);
  open($fh, ">" , $filename);
  Log::var("\$fh=$fh");
  if ($fh)
  {
    print $fh $transfered;
    close($fh);
  }
  Log::endFunction();
  return $cookie;
}


sub _get_transfered_cookie_file_name
{
   Log::startFunction();
   my ($self, $session) = @_;
   my $query = $self->query();   
   my $filename = "/tmp/_transfered_cookie_" . $session->{_session_id};
   my $process_id = undef;
   
  ### Get the right process id that comes from CGI parameter 
#    $process_id = $query->param("process_id");
#    if ($process_id)
#    {
#       $filename .=  '_' . $process_id;
#    }
   Log::var("\$filename=$filename \$process_id=$process_id");
   Log::endFunction();
   return $filename;
}

sub _remove_transfered_cookie_file
{
   Log::startFunction();
   my ($self, $session) = @_;
   my $filename = $self->_get_transfered_cookie_file_name($session);
   `/bin/rm -f $filename`;
   my $ret = $?;
   Log::var("\$ret=$ret");
   Log::endFunction();
   return $filename;
}


sub _get_transfered_cookie {
    Log::startFunction();
    my ($self, $session) = @_;
    my $query = $self->query();
    my $transfered_size = $query->cookie($TransWeb::COOKIE_NAME . "_TRANSFERED");
    
    my $fh = undef;
    my $filename = $self->_get_transfered_cookie_file_name($session);
    open($fh, "<" , $filename);
    Log::var("\$fh=$fh");
    if ($fh)
    {
       $transfered_size = <$fh>;
    }  
    Log::var("\$transfered_size=$transfered_size");
    Log::endFunction();
    return $transfered_size;
}

sub _get_ip_address {
  my ($self) = @_;
  my $query = $self->query();
  my $remote_host = $query->remote_host();
  my $ip_addr;
  if ( $remote_host =~ m/^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$/ ) {
    $ip_addr = $remote_host; # we have an ip address already
  } else {
    eval {
      $ip_addr = gethostbyname($remote_host);
    };
    $ip_addr = $ip_addr || '127.0.0.1';
  }
  #warn "IP: $ip_addr\n";
  return $ip_addr;
}



sub _set_intercom_interface {
  my ($self, $user_id, $user_name, $friendly_email) = @_;
  my $ip_addr = $self->_get_ip_address();
  my $ii = IntercomInterface->new($user_id,$user_name,$friendly_email,$ip_addr);
  $self->{INTERCOM_INTERFACE} = $ii;
}

sub _get_intercom_interface { 
  return $_[0]->{INTERCOM_INTERFACE}}

# ###########################################################
# helper functions
# ###########################################################

sub _set_uid_name {
  my ($self,$uid,$name,$friendly_email) = @_;
  $self->{uid} = $uid;
  $self->{name} = $name;
  $self->{friendly_email} = $friendly_email;
}

=pod

Get the currently logged in user_id and user_name for the 
IntercomInterface object, these values should also be present
in the CGI_Session

=cut

#sub get_uid_name {
#  my $self = shift;
#  my $ii = $self->_get_intercom_interface();
#  my ($uid,$name);
#  if ($ii) {
#    $uid = $ii->get_uid();
#    $name = $ii->get_name();
#  }
#  return ($uid,$name);
#}

sub get_uid_name { return ($_[0]->{uid},$_[0]->{name}) }


sub get_user_keys {
  my $self = shift;
  my $ii = $self->_get_intercom_interface();
  my $keys_array_ref;
  if ($ii) {
    $keys_array_ref = $ii->get_user_keys();
  }
  my @keys = ();
  if ($keys_array_ref) {
    @keys = @$keys_array_ref;
  }
  return @keys;
}

sub get_secure_keys {
  my $self = shift;
  my $ii = $self->_get_intercom_interface();
  my $keys_array_ref;
  if ($ii) {
    $keys_array_ref = $ii->get_secure_keys();
  }
  my @keys = ();
  if ($keys_array_ref) {
    @keys = @$keys_array_ref;
  }
  return @keys;
}

sub get_repositories {
  Log::startFunction();
  my $self = shift;
  my $ii = $self->_get_intercom_interface();
  my $keys_array_ref;
  if ($ii) {
    $keys_array_ref = $ii->get_repositories();
  }

  my $keys;
  if ($keys_array_ref) {
    foreach my $row (@$keys_array_ref) {
      my ($name) = split /;/, $row;
      $keys->{$row} = $name;
    }
    $keys->{ORDERED_LIST} = $keys_array_ref;
  }
  Log::endFunction();
  return $keys;
}

sub get_companies {
  my $self = shift;
  my $keys_array_ref = shift;
  unless ( defined $keys_array_ref ) { 
  # Provide a way to supply a new or pruned set of companies
  # to the function. If no companies are provide, we use the
  # Intercom library to get the standard list
    my $ii = $self->_get_intercom_interface();
    $keys_array_ref;
    if ($ii) {
      $keys_array_ref = $ii->get_companies();
    }
  }
  my $keys;
  if ($keys_array_ref) {
    foreach my $row (@$keys_array_ref) {
      my $name = _friendly_company_name($row);
      $keys->{$row} = $name;
    }
    $keys->{ORDERED_LIST} = $keys_array_ref;
  }
  return $keys;
}

sub _friendly_company_name {
  my $company_entry = shift;
  my ($name) = split /;/, $company_entry;
  return $name;
}

sub get_file_information {
  Log::startFunction();
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  if ($TransWeb::ENVIRONMENT eq 'internal') {
    # We only need a primary keyword
    if (!$primary_keyword) {
      $primary_keyword = $secondary_keyword;
      $secondary_keyword = undef;
    }
  }

  my $ii = $self->_get_intercom_interface();

  # call status, history, info
  my ($status,$msg) = $ii->get_status($primary_keyword,$secondary_keyword);
  if ($status) {
    $status->{HISTORY} = $ii->get_history($primary_keyword,$secondary_keyword);
    $status->{SIZE} = $ii->get_size($primary_keyword,$secondary_keyword);
    $status->{INFO} = $ii->get_info($primary_keyword,$secondary_keyword);
    $status->{DELETED} = 'NO';
  }
  Log::endFunction();
  return ($status,$msg);
}

sub properly_formatted_keyword {
  # returns true if the keyword is follows the proper format
  # it doesn't know or care if the keyword really exists
  # user keywords are of the from aaaa1111a
  # while secure keywords are aaaa1111A
  # the last digit represents the repository
  my $keyword = shift;
  if ($keyword =~/[a-z]{4}\d{4}[a-z]{1}/i) {
    return 1;
  }
  return undef;
}

sub print_file_to_browser {
 Log::startFunction();
  # this function will directly print to STDOUT
  # that is generally not allowed by CGI::Applcation
  # so the run method must be overridden and told to allow
  # this, also this function will be responsible for 
  # writing the html headers
  my ($self,$primary_keyword,$secondary_keyword,$retrieve_compressed_flag, $delete_flag) = @_;
  my $ii = $self->_get_intercom_interface();

  if ($TransWeb::ENVIRONMENT eq 'internal') {
    if (!$primary_keyword) {
      $primary_keyword = $secondary_keyword;
      $secondary_keyword = undef;
    }
  }


  # undef in case of error
  my @results = 
    $ii->get_init($primary_keyword, $secondary_keyword, $retrieve_compressed_flag, $delete_flag);


  my $file_name = $results[0];
  if (!$file_name) { # undef file_name means error
    # the error was the second element passed back
    Log::endFunction();
    return (undef,@results);
  }

  my $size = $results[3];
  my $size_comp = $results[4];
  my $compress_flag = $results[1];
  # If retrieving as a compressed file add .gz to file name
  if ($retrieve_compressed_flag == DBYES && $file_name !~ m/\.gz$/i )
  {
      $file_name = $file_name . ".gz";
  }
  
  $| = 1;
  #set up for manual output
  $self->header_type('none'); # cgi-app will do nothing
  my $query = $self->query();

  my $should_get_compressed = undef;
  if ($retrieve_compressed_flag == DBYES || $compress_flag == DBNO)
  {
     $should_get_compressed = 1;
     $size = $size_comp;
  }

  
  # Be careful here, do not add any extra characters
  # after the header or they will get pre-fixed to the
  # start of your file attachment and that is definately
  # not desired
  my $file_pointer = $results[2];
  Log::var("size=$size \$file_pointer=$file_pointer");
 
  if ($size > 0 )
  {
       if (!$should_get_compressed) ## get uncompressed 
       {
	  print $query->header( -Content_Disposition =>   "attachment; filename=\"$file_name\"",
				-type=>'application/octet-stream',  
				-Content_Length => $size);
       }
       else
       {
           print $query->header( -Content_Disposition =>   "attachment; filename=\"$file_name\"",
				 -type=>'application/zip',  
				 -Content_Transfer_Encoding=>'binary',
				 -Content_Length => $size);
       }  
  }
  else
  {   
       print $query->header( -Content_Disposition =>   "attachment; filename=\"$file_name\"",
		             -type=>'application/octet-stream'); 
		       
  }
  
  my ($ret,$error);
# if want to keep it compressed or if it is not compressed,
# do not use routines to uncompress the data
  if ($should_get_compressed)
  {
     ($ret, $error) = _get_compressed_file($ii);
#    ($ret, $error) = Intercom::icom_get_uncompressed();
# Otherwise uncompress the data before downloaded
  }
  else
  {     
     ($ret,$error) = _get_compressed_file_and_uncompress($ii,$file_pointer);
  }

  # error
  if (!$ret) {
   Log::endFunction();
    return (undef,$error);
  }

  # success
  Log::endFunction();
  return ($file_name,$error);
}

sub _get_compressed_file_and_uncompress {
  Log::startFunction();
  my ($ii,$file_ptr) = @_;
  my $continue = 1;
  my $counter = 1;
  my $total_bytes_read = 0;
  my @perl_buffer = (1..4096);
  my $buffer = join('', @perl_buffer);

  binmode(STDOUT, ":raw");
  while ($continue) {
      my ($ret) = Intercom::icom_read($file_ptr, $buffer);
      Log::var("icom_read()=$ret counter=$counter");
      $counter++;    
      if ( $ret <= 0 )
      {
	  $continue = undef;
      }
      else
      {
        my $wr = syswrite(STDOUT, $buffer, $ret);
        $total_bytes_read += $ret;
        Log::var("total_bytes_read=$total_bytes_read \$wr=$wr");
      }
  }
  my @result = $ii->get_compressed_close($file_ptr);
  Log::endFunction();
  return @result
}



sub _get_compressed_file {
  Log::startFunction();
  my $ii = shift;
  my $continue = 1;
  my $bytes_read = 0;
  my $bytes_wrote = 0;
  my $counter = 1;
  my $total_bytes_read = 0;
  my @perl_buffer = (1..4096);
  my $buffer = join('', @perl_buffer);
  
  #Log::var("buffer length=", length($buffer));

  binmode(STDOUT, ":raw");
  
  while ($continue) {
    my ($read_size) = Intercom::get_string($buffer);
    Log::var("\$read_size=$read_size");
    if ( $read_size <= 0 )
    {
        $continue = undef;
    }
    else
    {
        my $wr = syswrite(STDOUT, $buffer, $read_size);
        $total_bytes_read += $read_size;
        Log::var("total_bytes_read=$total_bytes_read \$wr=$wr");
    }
  }
  my @result = $ii->get_close;
  Log::endFunction();
  return @result;
}


# get the PARAMS from the query object for the file upload
# returns hash ref of param -> value
sub get_file_params {
  my $self = shift;
  my $query = $self->query();
  my %file_params;
  my @params = qw(FILE_DESCRIPTION
                  DELETE_AFTER_RETRIEVAL_FLAG
                  USE_COMPRESSION_FLAG
                  COMPANY 
                  ALTERNATE_EMAIL_ADDRESSES
  SERVICE_REQUEST );
  foreach my $param (@params) {
    $file_params{$param} = $query->param($param);
  }

  $file_params{ALTERNATE_EMAIL_NOTIFICATION_FLAG} = 'YES';

  unless ( $file_params{USE_COMPRESSION_FLAG} ) {
    $file_params{USE_COMPRESSION_FLAG} = 'YES';
  }

  if ($file_params{ALTERNATE_EMAIL_ADDRESSES}) {
    # convert to comma seperated
    my $address_string = clean_up_email_addresses(
			      $file_params{ALTERNATE_EMAIL_ADDRESSES});

    $file_params{ALTERNATE_EMAIL_ADDRESSES} = $address_string;
  }
  
  my $file_information = $query->uploadInfo($query->param('FILE_NAME'));
  if ($file_information) {
    my $file_name = 'transweb_upload';
    if ($$file_information{'Content-Disposition'}) {
      my $content_disp = $$file_information{'Content-Disposition'};
      my ($parsed_name) = $content_disp =~ /filename=\"(.+)\"/i;
      my $os = determine_users_os($query);
      fileparse_set_fstype($os);
      my $base_name = basename($parsed_name);
      if ($base_name) {
	$file_name = $base_name;
      }

      if ($file_name !~ /\.(gz|z|zip|tgz)$/i && $file_params{USE_COMPRESSION_FLAG} eq 'NO') {
	# warn "File $file_name is not compressed file type\n";
	# warn "Only previously compressed files can skip compression\n";
	$file_params{USE_COMPRESSION_FLAG} = 'YES';
      }
    }
    $file_params{FILE_NAME} = $file_name;
  }

  return \%file_params;
}

sub determine_users_os {
  my $query = shift;
  return undef unless $query;
  my $agent = $query->user_agent();
  if ($agent =~ /win/i) {
    return 'MSWin32';
  } elsif ($agent =~ /(unix|linux)/i) {
    return 'UnixOS';
  } elsif ($agent =~ /mac/i) {
    return 'MacOS';
  } else {
    return undef;
  }
}

#
# Take a block of text that can have e-mail addresses
# seperated by commas, new-lines, or white space
# and make those addresses a comma seperated string
sub clean_up_email_addresses {
  my $address_string = shift;
  my @address = split /,|\s+|\n/, $address_string;
  my @cleaned;
  foreach my $a (@address) {
    next unless $a;
    if ($a =~ /\w@\w/) {
      push @cleaned,$a;
    }
  }
  my $addresses = join(',',@cleaned);
  return $addresses;
}

# make sure we have everything we need to load the file
# returns which parameters are still needed, undef if none are needed
# hash ref
sub verify_file_params {
  my ($self,$file_params) = @_;
  my %still_needed;
  my @needed = qw(FILE_NAME FILE_DESCRIPTION COMPANY);

  if ($TransWeb::ENVIRONMENT eq 'external') {
    push @needed, 'ALTERNATE_EMAIL_ADDRESSES';
  }

  foreach my $need (@needed) {
    unless ($file_params->{$need}) {
      # we still need this one
      $still_needed{$need} = 1;
    }
  }
  return \%still_needed;
}

# load the file
# returns undef on error transfered bytes on success
sub upload_file {
  my ($self,$file_params) = @_;
  my $query = $self->query();
  my $ii = $self->_get_intercom_interface();
  my $session = $self->get_session();
  my $upload_fh = $query->upload('FILE_NAME');
  my ($result,$error_msg);

  # check the preconditions
  # we need a filehandle
  unless ($upload_fh) {
    return (undef,'NO FILE HANDLE FOR INTERCOM TRANSFER');
  }
  ### FILE COMPRESSION is now mandatory
  ###  if ($file_params->{USE_COMPRESSION_FLAG} eq 'YES') {
    ($result,$error_msg) = $self->_upload_file_compressed($upload_fh,
						  $file_params,
						  $ii,
						  $session);
  ### } else { # no compression
  ###  ($result,$error_msg) = _upload_file_uncompressed($upload_fh,
  ###						     $file_params,
  ###						     $ii,
  ###						     $session);
  ### }
  # return undef on error
  # return {KEYWORD,SIZE,FILE_NAME,COMPANY} on success
  if ($result) {
    return $result,$error_msg; # a hash ref of KEYWORD,SIZE,FILE_NAME,COMPANY
  }
  return (undef, $error_msg);
}

sub _upload_file_uncompressed {
  my ($file_handle,$file_params,$ii,$session) = @_;
  my $file_keyword = $ii->put_init($file_params);

  # make sure the keyword came back
  unless ($file_keyword) {
    return (undef,'INTERCOM FILE TRANSFER INITIALIZATION ERROR');
  }

  my $bytesread; 
  my $return;
  my $size = 0;

  while ($bytesread=sysread($file_handle,my $buffer, 1024)) {
    $return = Intercom::put_binary_string($buffer,$bytesread);
    $size += $bytesread;
    $session->{'BYTES_TRANSFERRED'} = $size;
  }
  
  my $msg = $ii->put_close($size);
  if ($return >= 0 && $msg) {

    $session->{'TRANSFER_IN_PROGRESS'} = 2;
    
    return ({
	    FILE_KEYWORD=>$file_keyword,
	    SIZE=>$size,
	    FILE_NAME=>$file_params->{FILE_NAME},
	    COMPANY=>_friendly_company_name($file_params->{COMPANY}),
	   },$msg);
  } # end return == 0 && ...
  # error during transfer
  # we can track this as well

  $session->{'TRANSFER_IN_PROGRESS'} = -1;
  return (undef,'ERROR DURING FILE TRANSFER TO INTERCOM SERVER');
}

sub _upload_file_compressed {
  Log::startFunction();
  my ($self, $file_handle,$file_params,$ii,$session) = @_;
  my ($file_keyword,$file_pointer) = $ii->put_compressed_init($file_params);

  # make sure the keyword came back
  unless ($file_keyword && $file_pointer) {
    # error during init

    $session->{'TRANSFER_IN_PROGRESS'} = -1;
    Log::endFunction();
    # in the event of an error the file_pointer is the error message
    return (undef,$file_pointer);
  }

  my $bytesread;
  my $return;
  my $size = 0;
  $session->{'TRANSFER_IN_PROGRESS'} = 1;
  while ($bytesread=sysread($file_handle,my $buffer, 1024)) {    
    $return = Intercom::icom_write($file_pointer,$buffer,$bytesread);
    $size += $bytesread;
    $session->{'BYTES_TRANSFERRED'} = $size; 
    $self->_bake_transfered_cookie($size, $session);
    Log::var("session addr = ", \$session, Dumper($session));
  }
  my $msg = $ii->put_compressed_close($file_pointer,
				      $file_params->{FILE_DESCRIPTION});
  if ($return >= 0 && $msg) {

    $session->{'TRANSFER_IN_PROGRESS'} = 2;
    Log::endFunction();
    return ({
	    FILE_KEYWORD=>$file_keyword,
	    SIZE=>$size,
	    FILE_NAME=>$file_params->{FILE_NAME},
	    COMPANY=>_friendly_company_name($file_params->{COMPANY}),
	   },$msg);
  } # end return == 0 && ...
  # error during transfer
  # we can track this as well

  $session->{'TRANSFER_IN_PROGRESS'} = -1;
  Log::endFunction();
  return (undef,'ERROR DURING FILE TRANSFER TO INTERCOM SERVER');
}

sub email_keywords {
  my ($self,$primary_keyword,$secondary_keyword,
      $email_addys,$desc) = @_;
  my $ii = $self->_get_intercom_interface();
  my $clean_addys = clean_up_email_addresses($email_addys);

  if (!keyword_present($primary_keyword,$secondary_keyword)) {
    $ERROR_MSG = "No keyword given\n";
    return undef;
  }

  if (!$clean_addys) {
    warn "Email Address: $email_addys\n";
    warn "Clean Address: $clean_addys\n";
    $ERROR_MSG = "Email keyword input error\n";
  }

  return $ii->email_keyword($primary_keyword,$secondary_keyword,
			    $clean_addys,$desc);
}

sub extend_keyword {
  my ($self,$primary_keyword,$secondary_keyword,$days) = @_;
  my $ii = $self->_get_intercom_interface();
  $days = $days || 3;

  if (!keyword_present($primary_keyword,$secondary_keyword)) {
    $ERROR_MSG = "No keyword given\n";
    return undef;
  }

  return $ii->extend_keyword($primary_keyword,$secondary_keyword,$days);
}

sub delete_keyword {
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  my $ii = $self->_get_intercom_interface();

  if (!keyword_present($primary_keyword,$secondary_keyword)) {
    $ERROR_MSG = "No keyword given\n";
    return undef;
  }

  return $ii->delete_keyword($primary_keyword,$secondary_keyword);
}


sub toggle_keyword_lock {
  my ($self,$primary_keyword,$secondary_keyword,$current_status) = @_;
  return undef unless ($primary_keyword && $current_status);
  if ($current_status =~ /YES/i) {
    return $self->unlock_keyword($primary_keyword,$secondary_keyword);
  } else {
    return $self->lock_keyword($primary_keyword,$secondary_keyword);
  }
}


sub lock_keyword {
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  my $ii = $self->_get_intercom_interface();

  if (!keyword_present($primary_keyword,$secondary_keyword)) {
    $ERROR_MSG = "No keyword given\n";
    return undef;
  }

  return $ii->lock_keyword($primary_keyword,$secondary_keyword);
}

sub unlock_keyword {
  my ($self,$primary_keyword,$secondary_keyword) = @_;
  my $ii = $self->_get_intercom_interface();

  if (!keyword_present($primary_keyword,$secondary_keyword)) {
    $ERROR_MSG = "No keyword given\n";
    return undef;
  }

  return $ii->unlock_keyword($primary_keyword,$secondary_keyword);
}

sub toggle_file_email {
  my ($self,$primary_keyword,$secondary_keyword,$current_status) = @_;
  return undef unless ($primary_keyword && $current_status);
  my $ii = $self->_get_intercom_interface();
  if ($current_status =~ /YES/i) {
    # cancel notification
    return $ii->can_file_email($primary_keyword,$secondary_keyword);
  } else {
    # set notification
    return $ii->set_file_email($primary_keyword,$secondary_keyword);
  }

}

sub toggle_user_email {
  my ($self,$current_status) = @_;
  return undef unless ($current_status);
  my $ii = $self->_get_intercom_interface();
  if ($current_status =~ /YES/i) {
    # cancel notification
    return $ii->can_user_email();
  } else {
    # set notification
    return $ii->set_user_email();
  }
}

=pod

keyword shortcut allows the user to type
[ URL ]?keyword as a shortcut to get to the 
get_start page. This is required to make
authentication work when a non-autheticated
user wants to get a file.
The user types the shortcut, we redirect to
the sign-in, the sign-in redirects to the original location

=cut

sub keyword_shortcut {
  my $self = shift;
  my $query = $self->query();
  my @keywords = $query->keywords(); 
  # in CGI programming there are PARAMS and KEYWORDS
  # params are URL?PARAM=VALUE&PARAM=VALUE
  # keywords are URL?KEYWORD+KEYWORD+KEYWORD
  # we allow the user to say URL?KEYWORD where one and only one CGI
  # keyword can be a TranWeb secure keyword

  if (@keywords && properly_formatted_keyword($keywords[0])) {
    my $keyword = $keywords[0];
    my $secure_keyword; # Ends with capital letter, primary
    my $file_keyword; # Ends with lowercase letter, secondary
    if ( $keyword =~ /^[a-z]{4}\d{4}[a-z]$/ ) { $file_keyword = $keyword }
    if ( $keyword =~ /^[a-z]{4}\d{4}[A-Z]$/ ) { $secure_keyword = $keyword }
    my @tw_keywords = ($secure_keyword,$file_keyword);
    return @tw_keywords;
  }
  return undef;
}

=pod

Test to see if there is a keyword for an internal site, or a
primary and secondary keyword for an external site

=cut
sub keyword_present {
  my ($primary_keyword,$secondary_keyword) = @_;

  if ($TransWeb::ENVIRONMENT eq 'internal') {
    # INTERNAL SITE
    if (
	properly_formatted_keyword($primary_keyword)
	||
	properly_formatted_keyword($secondary_keyword)
       ) {
      return 1; # need one or the other for internal
    }
  } else {
    # EXTERNAL SITE
    if (
	properly_formatted_keyword($primary_keyword)
	&&
	properly_formatted_keyword($secondary_keyword)
       ) {
      return 1; # need both for external
    }
  }
  return undef;
}
  

1;
