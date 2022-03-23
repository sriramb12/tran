#ifdef TRANSAPI
#ifdef INTERCOM
#ifdef NXP 
/****************************************************************************
 *
 * Function:	trans_help_print
 *
 * Description:	Prints help for NXP customers for InterCom.
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "global_defines.h"
#include "rtn.h"
#include "trans_client_errs.h"
#include "trans_defines.h"
#include "global_debug.h"

/* Used for NXP InterCom */
int trans_help_print(char *ptr, char command, char help_option)
{
    DBG();

switch(command)
{
case 'a': sprintf(rtn.msg, "%s -a    Print a help screen for advanced commands.\n\n", ptr); break;
case 'A': sprintf(rtn.msg, "%s -A    Display advanced put/get options.\n\n", ptr); break;
case 'b': sprintf(rtn.msg, "%s -b  keyword\n\n\
     Display list of files in directories for data with specified keyword.\n\n",
	ptr); break;
case 'B': sprintf(rtn.msg, "%s -b  keyword\n\n\
     Retrieve list of files in directories for data with specified keyword.\n\
     The list is stored in a file called trans_dirlist.txt.\n\n",
	ptr); break;
case 'd': sprintf(rtn.msg, "%s -d keyword\n\n\
    Delete file or directory associated with the  specified\n\
    keyword.  This command is executed after all users have\n\
    retrieved a copy of the data.  If one of the users with\n\
    the  file  keyword  does  not  delete  the  data, it is\n\
    automatically deleted three working days after  it  was\n\
    transferred thus keeping the disk free for the transfer\n\
    of other large files.\n\n", ptr); break;
case 'D':
     sprintf(rtn.msg, "%s -D keyword\n\n\
     Set delete lock for file or directory  associated  with\n\
     the  specified  keyword.  Only the user who transcended\n\
     the data, referred to  as  the  owner,   can  use  this\n\
     option.  When the delete lock is set, no one except the\n\
     owner can delete the data.  This option is useful  when\n\
     a  user  is distributing data to more than one user and\n\
     wants to prevent one of the  users  from  deleting  the\n\
     data  before  all of the users have retrieved the data.\n\
     However, even with the delete lock  set,  the  data  is\n\
     still  deleted by the Transcend audits when the expira-\n\
     tion date is reached.\n\n", ptr); break;
case 'g': sprintf(rtn.msg, "%s -g keyword\n\n\
    Retrieve the file or directory from InterCom associated\n\
    with  the  specified keyword.  Data retrieved is placed\n\
    in the user's current working directory.  If  the  data\n\
    is  already  present,  the user is asked if he wants to\n\
    overwrite the existing data.\n\n \
%s -gl keyword\n\n\
          Download zipped data  using  parallel  transfer.   When\n\
          this  option  is  used,  the  data  being downloaded is\n\
          divided into 10 logical blocks, and the 10  blocks  are\n\
          transferred  in  parallel.   In  most  cases,  parallel\n\
          transfer is faster than serial transfer.  the  increase\n\
          in  transfer  speed varies based on the file size, band\n\
          width of the data line, amount of traffic on  the  net-\n\
          work,  etc.  Parallel transfer is done for zipped files\n\
          greater than 1 Gb in size.  Parallel  transfer  can  be\n\
          done  for  zipped  files  greater  than 5 Mb in size by\n\
          using the -gl option or changing the default  size  for\n\
          parallel  transfer  in the .icomrc file.  This file can\n\
          be downloaded from the InterCom release directory  (see\n\
          the  -r  option)  and placed in the home directory.  If\n\
          present, values  from  this  file  are  used.   If  not\n\
          present, default values are used.\n\n\
%s -gr keyword\n\n\
          Retrieve a zipped file using the  retry  option.   this\n\
          option  is  useful  for very slow data lines where time\n\
          outs may occur.  When this option is used, the file  is\n\
          divided into multiple logical partitions, and each par-\n\
          tition is transferred as a separate block.  If transfer\n\
          of  any of the individual blocks fails, the transfer is\n\
          tried two additional times.  The retry  option  can  be\n\
          used  for files greater than 5 Mb in size.  The default\n\
          size for separate blocks is 5 Mb.  This  value  can  be\n\
          modified  by  changing the default value in the .icomrc\n\
          file.\n\n\
%s -gs keyword\n\n\
          Retrieve the data using serial  transfer.   All  direc-\n\
          tories  and  non zipped files are transferred in serial\n\
          mode.  Zipped files greater  than  1  Gb  in  size  are\n\
          automatically   transferred  using  parallel  transfer.\n\
          Serial transfer of these files is performed if the  -ps\n\
          option is used.\n\n\
%s -gt keyword\n\n\
          Force directory unbundling using tar.  during directory\n\
          transfer,  Bundling  of  the  data is done with gtar if\n\
          found using the  whereis  command.   Otherwise  tar  is\n\
          used.   If problems with gtar are encountered, The user\n\
          can force tar to be used by using the -gt option.\n\n",
	ptr, ptr, ptr, ptr, ptr); break;
case 'h': sprintf(rtn.msg, "%s -h keyword | mask set\n\n\
          Retrieve history for file or directory associated  with\n\
          the  specified keyword.  A summary of all commands per-\n\
          formed on the data is shown.  History can be  retrieved\n\
          for  up to two weeks after the original expiration date\n\
          of a file or directory.\n\n\
          If a mask set name  is  specified,  a  history  of  the\n\
          transfers  performed  for  the  mask  set is retrieved.\n\
          Files containing a history of the transfers are present\n\
          in  a  directory  called <mask>.history where <mask> is\n\
          the name of the specified  mask  set.   A  file  called\n\
          'files_sent'  contains  an  alphabetical listing of all\n\
          .cflt, .cref, and pattern files sent for the mask  set,\n\
          and  individual  files  of  the  form  mm_dd_yy contain\n\
          information about separate  transfers  performed.   The\n\
          history of mask set transfers is never deleted from the\n\
          InterCom repository.\n\n",
ptr); break;
case 'i': sprintf(rtn.msg, "%s -i keyword\n\n\
    Retrieve information about a file or directory  associ-\n\
    ated with the specified keyword.  Information retrieved\n\
    includes file name, size,  creator,  date  transferred,\n\
    and expiration date.\n\n", ptr); break;
case 'I': sprintf(rtn.msg, "%s -I    Display defined iCAP classifications.\n\n", ptr); break;
case 'j': sprintf(rtn.msg, "%s -j customer-e-mail \n\n\
    Request approval for specified customer.", ptr); break;
case 'J': sprintf(rtn.msg, "%s -J keyword    Display file name for specified keyword.\n\n", ptr); break;
case 'L': sprintf(rtn.msg, "%s -L keyword\n\n\
    Lock data to prevent retrieval.  This command is useful\n\
    when  the  user has placed a file/directory in InterCom\n\
    and wants to wait for a period of time before  allowing\n\
    other users to retrieve the data.\n\n", ptr); break;
case 'p': sprintf(rtn.msg, "%s -p -company file | directory [email...]\n\n\
    Put a file or directory  in  the  InterCom  repository.\n\
    Files   are   compressed   during   data  transfer  and\n\
    uncompressed during data  retrieval.   Directories  are\n\
    tarred   and   compressed   during  data  transfer  and\n\
    uncompressed and untarred during data retrieval.  After\n\
    data  transfer,  the user is given a randomly generated\n\
    9 character  file  keyword   he   can   give   to   any\n\
    user[s]  wishing  to retrieve the data.  If one or more\n\
    email addresses are specified, a secure keyword is sent\n\
    to  the specified email addresses.  Secure keywords are\n\
    valid only for the users who receive  them  via  email.\n\
    Both  file  and  secure  keywords  contain 4 lower case\n\
    alphabetic characters (a-z), followed by  four  numeric\n\
    characters  (0-9),  and  terminated  with an alphabetic\n\
    character.\n\n\
          during directory transfer, Bundling of the data is done\n\
          with gtar or gnutar if found using the whereis command.\n\
          Otherwise tar is used.  If problems with gtar or gnutar\n\
          are  encountered,  The user can force tar to be used by\n\
          using the '-pt' option instead of the '-p' option.\n\n\
%s -pl -company file [email...]\n\n\
          Upload zipped data using parallel transfer.  When  this\n\
          option  is  used,  the  file is divided into 10 logical\n\
          blocks, and the 10 blocks are transferred in  parallel.\n\
          In  most cases, parallel transfer is faster than serial\n\
          transfer.  the increase in transfer speed varies  based\n\
          on  the  file size, band width of the data line, amount\n\
          of traffic on the network, etc.  Parallel  transfer  is\n\
          automatically  done  for zipped files greater than 1 Gb\n\
          in size.  Parallel transfer  can  be  done  for  zipped\n\
          files greater than 5 Mb in size by using the -pl option\n\
          or changing the default size for parallel transfer   in\n\
          the  .icom.rc  file.   This file can be downloaded from\n\
          the InterCom release directory (see the -r option)  and\n\
          placed  in the home directory.  If present, values from\n\
          this file are used.  If not present, default values are\n\
          used.\n\n\
%s -pr -company file [email...]\n\n\
          Upload a zipped file  using  the  retry  option.   this\n\
          option  is  useful  for very slow data lines where time\n\
          outs may occur.  When this option is used, the file  is\n\
          divided into multiple logical partitions, and each par-\n\
          tition is transferred as a separate block.  If transfer\n\
          of  any of the individual blocks fails, the transfer is\n\
          tried two additional times.  The retry  option  can  be\n\
          used  for files greater than 5 Mb in size.  The default\n\
          size for separate blocks is 5 Mb.  This  value  can  be\n\
          modified  by  changing the default value in the .icomrc\n\
          file.\n\n\
%s -ps -company file [email...]\n\n\
          Upload the data using serial transfer.  All directories\n\
          and  non  zipped  files are transferred in serial mode.\n\
          By default, zipped files greater than 1 Gb in size  are\n\
          automatically   transferred  using  parallel  transfer.\n\
          Serial transfer of these files is performed if the  -ps\n\
          option is used.\n\n",
ptr, ptr, ptr, ptr); break;
case 'E':
     sprintf(rtn.msg, "%s -EEEE _k_e_y_w_o_r_d\n\n\
          Resend encrypted data to  the foundry  via  TransFTP.   This\n\
          option is used when data is being transferred to the foundry,\n\
          and  the  data  was  successfully  transferred  to  the\n\
          TransFTP  repository  and  encrypted using GPG, but was\n\
          not successfully transferred to the remote company  via\n\
          FTP.   This  option  allows  the  user  to  resend  the\n\
          encrypted data to the remote company via FTP, but skips\n\
          the  steps  of sending the data to the TransFTP reposi-\n\
          tory and encrypting the data.\n\n", ptr); break;
case 'F': sprintf(rtn.msg, "%s -F -company file... | directory...\n\n\
    Put multiple files or directories in the InterCom repo-\n\
    sitory and delete after the first successful retrieval.\n\
    The  files  are  tarred  and  compressed  during   data\n\
    transfer  and  untarred  and  uncompressed  during data\n\
    retrieval.  The files and directories are  put  in  the\n\
    receiving  user's current working directory as separate\n\
    files and directories.   Wild  cards  can  be  used  to\n\
    specify the file names.\n\n", ptr); break;
case 'G': sprintf(rtn.msg, "%s -G keyword\n\n\
    Get the file or directory from InterCom associated with\n\
    the  specified  keyword  and delete from InterCom after\n\
    the current retrieval.\n\n", ptr); break;
case 'H': sprintf(rtn.msg, "%s -H -command\n\n\
    Display detailed help for specified command i.e.  \n\
    'icom -H -p'\n\n", ptr); break;
case 'K': sprintf(rtn.msg, "%s -K -company directory file...\n\n\
    Package files listed  on  the  command  line  into  the\n\
    specified  directory,  put  in the InterCom repository,\n\
    and delete after the first successful retrieval.   This\n\
    option  is like the -F option except the files are bun-\n\
    dled together in a directory during data  transfer  and\n\
    are  retrieved  in the directory during data retrieval.\n\
    The directory the files are placed  in  is  a  symbolic\n\
    directory and does not need to be present on the user's\n\
    disk.  Wild cards can  be  used  to  specify  the  file\n\
    names.   Only  files  can  be  placed in the directory.\n\
    Other directories cannot be included.\n\n", ptr); break;
case 'N': sprintf(rtn.msg, "%s -N -company file-list [email...]\n\n\
    Transfer the files listed in the named file and  delete\n\
    after  the  first successful retrieval.  This option is\n\
    like the -F option except the file names are listed  in\n\
    a  file  rather than on the command line.  Names within\n\
    the file are put on separate lines.  Wild cards  cannot\n\
    be  used within the file, and directory names cannot be\n\
    included.  Secure keywords can be  sent  to  multiple  users\n\
    (see the -m option).\n\n", ptr); break;
case 'o':  sprintf(rtn.msg, "%s -o [-site]\n\n\
     Obtain user's registration status.  This feature is \n\
     useful if the user wants to check his registration status\n\
     without performing registration.\n\n", ptr); break;
case 'O': sprintf(rtn.msg, "%s -O  [-site] file | directory [email...]\n\n\
          Transfer duplicate files to the mask shop.  Checks  are\n\
          in  place to prevent duplicate files from being sent to\n\
          the mask shop.  Using this option overrides the  checks\n\
          in place and allows duplicate files to be transferred.\n\n", ptr); break;
case 'P': sprintf(rtn.msg, "%s -P -company file | directory [email...]\n\n\
    Put a file or directory in InterCom  and  delete  after\n\
    the  first  successful  retrieval.  This option is used\n\
    when a file will be retrieved by only one user.\n\n", ptr); break;
case 'Q': sprintf(rtn.msg, "%s -Q | -Ql\n\n\
          Display list of secure keywords received.  The keywords are shown in\n\
              reverse chronological order, and the symbol '+' or '-' indicates\n\
              if the file data is still present in the repository.  If the -Ql\n\
              option  is  used, a long list of secure keywords for data the user\n\
              has received is displayed.  This  list  includes  a  history  of\n\
              transactions  performed for each keyword.  If the user has write\n\
              permission in his current working directory, the  list  of  key-\n\
              words is saved in a file called 'secure_keywords'.\n\
          Only   secure   keywords  associated  with  the  user's\n\
          registered primary e-mail address  are  shown.   Secure\n\
          keywords  sent  to other users are not shown.  This  feature  is  \n\
          useful when the user has lost or has not received e-mail messages with\n\
          secure  keywords  from  other  users.   This  option is\n\
          identical to the '-V' option.\n\n", ptr); break;
case 'R':
     sprintf(rtn.msg, "%s -R [-site] Core ID [Active directory Password]\n\n\
          Perform InterCom registration.  This option  is  useful\n\
          when a user wants to register without executing another\n\
          command.  If the user does not enter his Active Directory\n\
          Password  on  the  command line, he is prompted for the\n\
          password, and the password is not echoed to the screen.\n\n", ptr); break;
case 'T': sprintf(rtn.msg, "%s -T -company directory [email...]\n\n\
    Tar a directory tree following symbolic links, transfer\n\
    to  the InterCom repository, and delete after the first\n\
    successful retrieval.  This option  is  different  from\n\
    the  -P  option because symbolic links are not followed\n\
    with the -P option.\n\n", ptr); break;
case 'U': sprintf(rtn.msg, "%s -U keyword\n\n\
    Unlock data to allow data retrieval.   This  option  is\n\
    used  to  unlock  data  previously locked for retrieval\n\
    with the '-L' option.\n\n", ptr); break;
case 'V': sprintf(rtn.msg, "%s -V | -Vl\n\n\
          Display list of secure keywords received.  The keywords are shown in\n\
              reverse chronological order, and the symbol '+' or '-' indicates\n\
              if the file data is still present in the repository.  If the -Ql\n\
              option  is  used, a long list of secure keywords for data the user\n\
              has received is displayed.  This  list  includes  a  history  of\n\
              transactions  performed for each keyword.  If the user has write\n\
              permission in his current working directory, the  list  of  key-\n\
              words is saved in a file called 'secure_keywords'.\n\
          Only   secure   keywords  associated  with  the  user's\n\
          registered primary e-mail address  are  shown.   Secure\n\
          keywords  sent  to other users are not shown.  This  feature  is  \n\
          useful when the user has lost or has not received e-mail messages with\n\
          secure  keywords  from  other  users.   This  option is\n\
          identical to the '-Q' option.\n\n", ptr); break;
case 'X': 
     sprintf(rtn.msg, " %s -XXXX   Display  defined  InterCom repositories.\n\n", ptr); break;
case 'Z': sprintf(rtn.msg, "%s -Z -company directory file-list [email...]\n\n\
    Zip/package files listed  in  the  named  file  into  a\n\
    directory  transfer  to  the  InterCom  repository, and\n\
    delete after  the  first  successful  retrieval.   This\n\
    option  is like the -K option except the file names are\n\
    listed in a file instead of on the command line.   File\n\
    names  within  the  file must be put on separate lines.\n\
    Wild cards cannot  be  used  within  the  file.   Also,\n\
    directory  names  cannot be included in the file.  Like\n\
    the -K option, the files  are  bundled  together  in  a\n\
    directory  during  data  transfer  and retrieved in the\n\
    directory during data retrieval.  Secure  keywords  can\n\
    be sent to multiple users (see the -m option).\n\n", ptr); break;
case 'c': rtn.len = sprintf(rtn.msg, "%s -c keyword | all\n\n", ptr);
    sprintf(rtn.msg+rtn.len, "Cancel previously set email notification.  If a keyword\n\
    is  specified, email notification is cancelled only for\n\
    the data associated with  the  keyword.   If  the  word\n\
    'all'  is used, email notification is cancelled for all\n\
    files the user has placed in InterCom except those  for\n\
    which  email notification has been explicitly set using\n\
    the file keyword.\n\n"); break;
case 'C': sprintf(rtn.msg, "Administrative command for deletion of files .\n\n"); break;
case 'e': sprintf(rtn.msg, "%s -e keyword [days]\n\n\
    Extend the expiration date on a file or directory asso-\n\
    ciated  with  the specified keyword  one to three addi-\n\
    tional working days.  If the  user  does  not  enter  a\n\
    value  for days on the command line, he is prompted for\n\
    a value.\n\n", ptr); break;
case 'f': sprintf(rtn.msg, "%s -f -company file... | directory...\n\n\
    Put files or directories listed  on  the  command  line\n\
    into  the  InterCom  repository.   The files and direc-\n\
    tories are tarred and compressed during  data  transfer\n\
    and  untarred  and  uncompressed during data retrieval.\n\
    The data is put in the receiving user's current working\n\
    directory  as  separate  files  and  directories.  Wild\n\
    cards can be used to specify the   file  and  directory\n\
    names.\n\n", ptr); break;
case 'k': sprintf(rtn.msg, "%s -k -company directory file...\n\n\
    Package files listed on the command line into a  speci-\n\
    fied  directory  and  put the directory in the InterCom\n\
    repository.  This option is like the -f  option  except\n\
    the  files  are  bundled together in a directory during\n\
    data transfer and are retrieved in the directory during\n\
    data  retrieval.  The directory the files are placed in\n\
    is a symbolic directory and does not need to be present\n\
    on  the user's disk.  Wild cards can be used to specify\n\
    the file names.  Only files can be placed in the direc-\n\
    tory.  Other directories cannot be included.\n\n", ptr); break;
case 'l': sprintf(rtn.msg, "%s -l keyword\n\n\
    Display original, compressed, and stored file length of\n\
    file  or  directory  associated with the keyword.  This\n\
    option provides a method for the user to verify data in\n\
    the  InterCom  repository  is the correct size.  If the\n\
    compressed and stored data size are 0, the data was not\n\
    successfully transferred.\n\n", ptr); break;
case 'm': sprintf(rtn.msg, "%s -m keyword email...\n\n\
          Mail multiple users a 9 character  secure  keyword  for\n\
          the  file  or  directory  associated with the specified\n\
          keyword.  Each user is specified by his complete inter-\n\
          net e-mail address i.e. transbug@nxp.com.  If\n\
          complete internet e-mail addresses are not used, secure\n\
          keywords  are  not  sent.   The secure keywords sent to\n\
          users may only be used by the  user  who  receives  the\n\
          keyword.   Secure  keywords  may be used to perform all\n\
          transactions file keywords are used for.   Secure  key-\n\
          words can be sent using the -N, -P, -T, -Z, -n, -p, -t,\n\
          and -z options.\n\n", ptr); break;
case 'n': sprintf(rtn.msg, "%s -n -company file-list [email...]\n\n\
     Transfer files listed in the named file to the InterCom\n\
     repository.   This  option is like the -f option except\n\
     the file names are listed in a file rather than on  the\n\
     command  line.   File names within the file must be put\n\
     on separate lines.  Blank lines cannot  be  present  in\n\
     the  file.   Wild cards cannot be used within the file,\n\
     and directory names cannot be  included.   Secure  key-\n\
     words may be sent to multiple users (see the -m option).\n\n", ptr); break;
case 'q': sprintf(rtn.msg, "%s      -q | -ql\n\n\
              Display list of file keywords for data user has uploaded to  the\n\
              InterCom  repository.   The list of keywords  is  shown  in\n\
              reverse chronological order, and the symbol '+' or '-' indicates\n\
              if the file data is still present in the repository.  If the -ql\n\
              option  is  used, a long list of file keywords for data the user\n\
              has uploaded is displayed.  This  list  includes  a  history  of\n\
              transactions  performed for each keyword.  If the user has write\n\
              permission in his current working directory, the  list  of  key-\n\
              words is saved in a file called 'file_keywords'.\n\n", ptr); break;
case 'r': sprintf(rtn.msg, "%s -r [release] | [document]\n\n\
    Display list of commands which can be used to  retrieve\n\
    InterCom releases and documents.  InterCom is available\n\
    on Solaris, Linux, and Windows.\n\n", ptr); break;
case 's': rtn.len = sprintf(rtn.msg, "%s -s all | keyword\n\n", ptr);
    sprintf(rtn.msg+rtn.len, "Set email notification for one  or  more  files.   When\n\
    email notification is set, the user receives email each\n\
    time one  of his files is retrieved from InterCom.   If\n\
    a  keyword  is used, email notification is set only for\n\
    the associated data.  If the word 'all' is used,  email\n\
    notification  is  set  for all of the user's data.  The\n\
    user may set Email notification only for  data  he  has\n\
    transferred.\n\n"); break;
	case 'S': sprintf(rtn.msg, "%s -s\n\n\
Display Intercom server status.  This command is used by the  \n\
GUI to verify supported servers are active.\n\n",
 ptr); break;
case 't': sprintf(rtn.msg, "%s -t -company directory [email...]\n\n\
    Tar a  directory  tree  following  symbolic  links  and\n\
    transfer  to  the  InterCom repository.  This option is\n\
    different from the '-p' option because  symbolic  links\n\
    are not followed with the '-P' option.\n\n", ptr); break;
case 'u': sprintf(rtn.msg, "%s      -uuuu[-_s_i_t_e]  _c_o_r_e _I_D\n\n\
          Display information about  user  retrieved  from  LDAP\n\
          directory.  The  user's name, e-mail address, telephone\n\
          number, department, location code, and  mail  drop  are\n\
          displayed.  Registered  InterCom  sites are also shown.\n\
          If the user is not present in the LDAP  directory,  no\n\
          information   is   returned  even  if  the  user  is  a\n\
          registered InterCom user.\n\n", ptr); break;
case 'v': sprintf(rtn.msg, "%s -v [-site]\n\n\
    Display program version, default server machine,  local\n\
    machine,  InterCom  log  directory,  etc.  If a site is\n\
    specified, information about the site is shown.\n\n", ptr); break;
case 'w':  sprintf(rtn.msg, "%s -w [-site]\n\n\
    Ping dropbox machine to verify it can be reached.\n\n", ptr); break;
case 'W':  sprintf(rtn.msg, "%s -W [-site]\n\n\
    Silent ping to dropbox machine to verify it can be reached.\n\
    This option can be run as a cron job to keep client/server VPN  \n\
    connections from going down.\n\n", ptr); break;
case 'x': sprintf(rtn.msg, "%s -x    Display defined InterCom external companies  The  company\n\
     name  and site abbreviation are shown.  \n\n",  ptr); break;
case 'z': sprintf(rtn.msg, "%s -z -company directory file-list [email...]\n\n\
Zip/package files listed in a named file  into  a  directory\n\
and  put  the  directory  in the Transcend repository.  This\n\
option is like the -k option except the files are listed  in\n\
a  named file rather than on the command line.  Names within\n\
the file must be put  on  separate  lines,  and  wild  cards\n\
cannot  be used within the file.  The files are bundled into\n\
a  directory  during  data  transfer  and  retrieved  as   a\n\
directory  during  data  retrieval.  The directory the files\n\
are placed in is a symbolic directory and does not  need  to\n\
be  present  on  the sending user's disk.  Only files can be\n\
placed  in  the  directory.   Other  directories  cannot  be\n\
included.   Secure  keywords  may  also be sent to multiple users\n\
(see the -m option).\n\n", ptr); break;
	case 'M': sprintf(rtn.msg, "%s -M\n\n\
     Release  shared  memory  blocks  no  longer being used.\n\
     This command is executed to clean up shared memory  not\n\
     properly  released  when  parallel  data  transfers are\n\
     aborted or interrupted.\n\n", ptr); break;
	case 'y':
 sprintf(rtn.msg, "%s        --yy _e_-_m_a_i_l _j_u_s_t_i_f_i_c_a_t_i_o_n\n\n\
              Approve external customer to retrieve data from NXP.\n\n", ptr); break;
	/* Display user LDAP entry */
case '1': sprintf(rtn.msg, "     %s -1 [-_s_i_t_e] _C_o_r_e _U_s_e_r _I_D | _f_i_r_s_t-_n_a_m_e._l_a_s_t-_n_a_m_e\n\
          Display information about NXP users with an  entry  in\n\
          the  LDAP  directory.   Data  displayed includes name,\n\
          phone,  department,  etc.  This command is identical to\n\
          the  'User  command' with the exception that registered\n\
          InterCom sites are not displayed.   this command  runs\n\
          faster  than the 'User command' because time to look up\n\
          the user in the InterCom registry is eliminated.\n\n",
ptr); break;
     case '*': sprintf(rtn.msg, "%s -IIII   Display customer commands.\n\n\
          Special commands requested by customers are present.\n\n", ptr); break;
default:
{
	sprintf(rtn.msg, help_err);
	return(HELP_ERR);
} /* End default */
} /* End switch */
    return(SUCCESS);
} /* end help_print */
#endif
#endif /* for INTERCOM */
#endif /* for TRANSAPI */
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
