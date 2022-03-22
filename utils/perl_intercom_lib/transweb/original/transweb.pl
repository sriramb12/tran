#!/usr/local/bin/perl
use strict;
$ENV{PATH} = {};

# find out which host we are running on
# the devel and production servers on the intranet
# use /var/www as the server root (RedHat Linux, i386)
# the production server on the extranet (Sun Blade 100)
# uses /usr/local/apache/ as the server root

use lib(
	'/apps/apache/apps/transweb/app/cgi-bin',
	'/apps/apache/apps/transweb/lib/perl5/site_perl/5.8.5',
	'/apps/apache/apps/transweb/lib/perl5/site_perl/5.8.5/sun4-solaris',
	'/apps/apache/apps/transweb/lib/perl5/site_perl/5.8.5/sun4-solaris/auto',
	);

use TransWeb;


$TransWeb::ENVIRONMENT = 'external'; # internal or external
$TransWeb::SESSION_DIRECTORY = '/tmp/transweb_cookies'; # Renee uses /tmp/transweb already
$TransWeb::TT_CACHE_DIR = '/tmp/transweb_cookies/tt_cache';
# For Cookies
$TransWeb::COOKIE_NAME = 'CGISESSID_TRANSWEB';
$TransWeb::COOKIE_EXPIRES = undef;
$TransWeb::COOKIE_PATH = '/';
# For Template Toolkit
$TransWeb::TEMPLATE_DIRECTORY = ['/apps/apache/apps/transweb/app/templates/'];
$TransWeb::DOCUMENTATION = '/apps/apache/apps/transweb/app/doc/external_transweb_documentation.pdf';
$TransWeb::STYLESHEET = "/shared/stylesheets/toplevel.css";

my $transweb_instance = TransWeb->new();
$transweb_instance->run();


