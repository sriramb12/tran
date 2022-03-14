package Log;

use strict;
use IO::Handle;
use CGI;
use File::Basename;
use Socket;
use Sys::Hostname;
use Cwd;
use Login;

our $debug_enabled;  ## 0/1 = true/false

my $level=0;
my $logOut=undef;
my $logFile;
my $fileCaller   = undef;
my $lineCaller  = undef;
my $fileCallerFullPath  = undef;



## if dir is a valid directory returns it, otherwise returns undef
sub check_valid_log_dir
{
    my $dir  = $_[0];
    my $ret = undef;
    my $cur_dir = getcwd();
    if (!chdir($dir))
    {
        `mkdir "$dir"`;
    }
    `chmod 777 "$dir" 2>/dev/null`;
    if (-W "$dir")
    {
        $ret  = $dir;
    }
    chdir($cur_dir);
    return $ret;
}


BEGIN {
          foreach (@INC) {
               if ( -e "$_/LogOn.pm"  ) {
                  $Log::packageDir = $_;
               }
          }
          $Log::debug_enabled = 1;

          ## prevent from running from production,  only on a path like 'dev'
          my $cgi = $ENV{'SCRIPT_FILENAME'};
#          if ($cgi && $cgi  !~ /dev/ ) {
#             $Log::debug_enabled = 0;
#          }

          my $logDir = undef;
          my $user =  Login::getUser();
          $logOut = undef;
          ## this file might contain the name of the log output file, if it does not exist the previous
          ## $Log::logFile will be used
          if ( -e "$Log::packageDir/LogpmOutput.txt"  )
          {
               open ( my $logname, "<$Log::packageDir/LogpmOutput.txt");
               $Log::logFile  = <$logname>;
               chomp ($Log::logFile);
               close($logname);
               $logDir = 1;  ## make it true to to open log file
          }
          else
          {
                $logDir =      check_valid_log_dir("/home/scratch/dtmsdb/logs")
                            || check_valid_log_dir("/scratch/dtmsdb/logs")
                            || check_valid_log_dir("/tmp/logs");
                my $host = hostname();
                $Log::logFile  = $logDir . "/${user}_${host}_" . "Logpm.log";
          }
          if ($logDir)
          {
                open($logOut, ">>" . $Log::logFile);
                chmod(0666, $logOut);
                close($logOut);
                `chmod 666 $Log::logFile    2>/dev/null`;
                `chown $user  $Log::logFile 2>/dev/null`;
                open($logOut, ">>" . $Log::logFile);
                print $logOut "\n\n----------- new LOG starts here ----------------------\n\n";
                $logOut->autoflush(1);
                close($logOut);
		$Log::debug_enabled = 1;
          }
          else
          {
             $Log::debug_enabled = 0;
          }
}


##
## Private function
## my $var = sub {};
##    $var-> ();


##  --------------- Private functions ----------------------


my $printLevel = sub
{
  print $logOut "\n";
  for (my $i = 0; $i < $level; $i++) {
         print $logOut " ";
  }
};


my $openLogAndPrintLevel = sub
{
  open($logOut, ">>" . $Log::logFile);
  $printLevel->();

};


my $closeLog  = sub
{
   $logOut->autoflush(1);
   close($logOut);
};




my $privateFunctionAction = sub
{
     my $inout = $_[0];
     my $func  = $_[1];

     $openLogAndPrintLevel->();

     if ( length($func) == 0 ) {
           $func = "::";
     }
     my $levelStr = sprintf("[%02d]", $level);

     print $logOut "$inout",  $levelStr,  " $func() ";
     if ( $inout eq ">>")
     {
        print $logOut "called from ";
     }
     else
     {
        print $logOut "finished in ";
     }
     print $logOut "[$fileCaller:$lineCaller]";
     $closeLog->();
};


##  --------------- Public functions ----------------------

sub  var
{
    if ( $Log::debug_enabled == 1 ) {
        $fileCallerFullPath = (caller(0))[1];
        $fileCaller   = $fileCallerFullPath ? basename($fileCallerFullPath) : $ARGV[0];
        $lineCaller   = (caller(0))[2];
        $openLogAndPrintLevel->();
        print $logOut "   [v][$fileCaller:$lineCaller] ",  @_;
        $closeLog->();
    }
}


sub startFunction()
{
  if ( $Log::debug_enabled == 1 ) {
     $fileCallerFullPath = (caller(1))[1];
     $fileCaller   = $fileCallerFullPath ? basename($fileCallerFullPath) : $ARGV[0];
     $lineCaller   = (caller(1))[2];
     my $func   = (caller(1))[3];
    $privateFunctionAction->(">>", $func);
    $level++;
  }
}


sub endFunction()
{
  if ( $Log::debug_enabled == 1 ) {
    $fileCallerFullPath = (caller(0))[1];
    $fileCaller   = $fileCallerFullPath ? basename($fileCallerFullPath) : $ARGV[0];
    $lineCaller   = (caller(0))[2];
    my $func   = (caller(1))[3];
    $level--;
    $privateFunctionAction->("<<", $func);
  }
}


sub myFileName()
{
  if ( $Log::debug_enabled == 1 ) {
     my $file   = (caller(1))[1];
     $openLogAndPrintLevel->();
     print $logOut "[file $file]";
     $closeLog->();
  }
}



sub cgiParamameters()
{
   if ( $Log::debug_enabled == 1 ) {
     my $file   = (caller(1))[1] || (caller(0))[1];
     $openLogAndPrintLevel->();
     print $logOut "[file $file]";
     my $query   = $dtmsGlobals::t;
     if (!$query) {
       $query = CGI->new();
     }
     my @params  = $query->param();
     foreach(@params) {
       $openLogAndPrintLevel->();
       my @value = $query->param($_);
       $printLevel->();
       print $logOut "CGI::param $_=@value";
     }
     $closeLog->();
   }
}


1;
