#ifdef TRANSAPI
#ifdef INTERCOM
#ifndef NXP 
/****************************************************************************
 *
 * Function:	trans_help_print
 *
 * Description:	Routines to print help for InterCom customers 
 *		who are at external companies.
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "global_defines.h"
#include "rtn.h"
#include "trans_client_errs.h"
#include "global_debug.h"

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
     Retrieve list of files in directories for data with specified keyword.\n\n",
	ptr); break;
case 'c': sprintf(rtn.msg, "%s -c keyword | all\n\n\
    Cancel previously set email notification.  If a keyword\n\
    is  specified, email notification is cancelled only for\n\
    the file or directory associated with the keyword.   If\n\
    the word 'all' is used, email notification is cancelled\n\
    for all files the user has placed  in  InterCom  except\n\
    those  for which email notification has been explicitly\n\
    set using the file keyword.\n\n", ptr); break;
case 'C': sprintf(rtn.msg, "Administrative command for deletion of files .\n\n"); break;
case 'd': sprintf(rtn.msg, "%s -d keyword\n\n\
          Delete file or directory associated with the  specified\n\
          keyword.  This command is executed after all users have\n\
          retrieved a copy of the data.  If one of the users with\n\
          the  keyword  does not delete the data, it is automati-\n\
          cally deleted nine days after it was  transferred  thus\n\
          keeping  the  disk free for the transfer of other large\n\
          files.  The number of days the file is kept is a server\n\
          configurable parameter.\n\n",
    ptr); break;
case 'D':
     sprintf(rtn.msg, "%s -D keyword\n\n\
     Set delete lock for file or directory  associated  with\n\
     the  specified  keyword.  Only the user who uploaded\n\
     the data, referred to  as  the  owner,   can  use  this\n\
     option.  When the delete lock is set, no one except the\n\
     owner can delete the data.  This option is useful  when\n\
     a  user  is distributing data to more than one customer and\n\
     wants to prevent one of the  customers  from  deleting  the\n\
     data  before  all of the users have retrieved the data.\n\
     However, even with the delete lock  set,  the  data  is\n\
     still  deleted by the InterCom audits when the expira-\n\
     tion date is reached.\n\n", ptr); break;
case 'e': sprintf(rtn.msg, "%s -e keyword [days]\n\n\
    Extend the expiration  date  on  a  file  or  directory\n\
    associated  with  the  specified  keyword  one to three\n\
    additional working days.  If the user does not enter  a\n\
    value  for days on the command line, he is prompted for\n\
    a value.\n\n", ptr); break;
case 'E':
printf("%s -E keyword\n\n\
     Cancel delete lock for the file or directory associated\n\
     with  the  specified  keyword.  Only the user who tran-\n\
     the data, referred to as the owner,  can cancel\n\
     the  delete lock.  This option is  used  when the owner\n\
     no longer needs to protect data from being  deleted  by\n\
     other users.\n\n", ptr); break;
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
case 'h': sprintf(rtn.msg, "%s -h keyword\n\n\
    Retrieve history for file or directory associated  with\n\
    the  specified keyword.  A summary of all commands per-\n\
    formed on the data is shown.  History can be  retrieved\n\
    for  up to two weeks after the original expiration date\n\
    of a file or directory.\n\n", ptr); break;
case 'i': sprintf(rtn.msg, "%s -i keyword\n\n\
          Retrieve information about a file or directory  associ-\n\
          ated with the specified keyword.  Information retrieved\n\
          includes file name, size,  creator,  date  transferred,\n\
          and expiration date.\n\n",
    ptr); break;
case 'I': sprintf(rtn.msg, "%s -I    Display defined iCAP classifications.\n\n", ptr); break;
case 'J': sprintf(rtn.msg, "%s -J keyword    Display file name for specified keyword.\n\n", ptr); break;
case 'p': sprintf(rtn.msg, "%s -p | -ps file | directory [email...]\n\n\
    Put a file or directory  in  the  InterCom  repository.\n\
    Files   are   compressed   during   data  transfer  and\n\
    uncompressed during data  retrieval.   Directories  are\n\
    tarred   and   compressed   during  data  transfer  and\n\
    uncompressed and untarred during data retrieval.  After\n\
    data  transfer,  the user is given a randomly generated\n\
    9 character  file  keyword   he   can   give   to   any\n\
    user[s]  wishing  to retrieve the data.  If one or more\n\
    email addresses are specified, a secure keyword is sent\n\
    to   the  specified  email addresses. Complete Internet\n\
    email addresses must  be  used.   Secure  keywords  are\n\
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
%s -pl file [email...]\n\n\
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
%s -pr file [email...]\n\n\
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
%s -ps file [email...]\n\n\
          Upload the data using serial transfer.  All directories\n\
          and  non  zipped  files are transferred in serial mode.\n\
          By default, zipped files greater than 1 Gb in size  are\n\
          automatically   transferred  using  parallel  transfer.\n\
          Serial transfer of these files is performed if the  -ps\n\
          option is used.\n\n",
ptr, ptr, ptr, ptr); break;
case 'F': sprintf(rtn.msg, "%s -F file... | directory...\n\n\
    Put multiple files or directories in the InterCom repo-\n\
    sitory and delete after the first successful retrieval.\n\
    The  files  are  tarred  and  compressed  during   data\n\
    transfer  and  untarred  and  uncompressed  during data\n\
    retrieval.  The files and directories are  put  in  the\n\
    receiving  user's current working directory as separate\n\
    files and directories.   Wild  cards  can  be  used  to\n\
    specify the file and directory names.\n\n", ptr); break;
case 'G': sprintf(rtn.msg, "%s -G keyword\n\n\
    Get the file or directory from InterCom associated with\n\
    the  specified  keyword  and delete from InterCom after\n\
    the current retrieval.\n\n", ptr); break;
case 'H': sprintf(rtn.msg, "%s -H -command\n\n\
    Display detailed help for specified command i.e.  \n\
    'icom -H -p'\n\n", ptr); break;
case 'K': sprintf(rtn.msg, "%s -K directory file...\n\n\
    Package files listed on the command line into a  speci-\n\
    fied  directory,  put  in  the InterCom repository, and\n\
    delete after  the  first  successful  retrieval.   This\n\
    option  is like the -F option except the files are bun-\n\
    dled together in a directory during data  transfer  and\n\
    are  retrieved  in the directory during data retrieval.\n\
    The directory the files are placed  in  is  a  symbolic\n\
    directory and does not need to be present on the user's\n\
    disk.  Wild cards can  be  used  to  specify  the  file\n\
    names.   Only  files  can  be  placed in the directory.\n\
    Other directories cannot be included.\n\n", ptr); break;
case 'L': sprintf(rtn.msg, "%s -L keyword\n\n\
    Lock data to prevent retrieval.  This command is useful\n\
    when  the  user has placed a file/directory in InterCom\n\
    and wants to wait for a period of time before  allowing\n\
    other users to retrieve the data.\n\n", ptr); break;
	case 'M': sprintf(rtn.msg, "%s -M\n\n\
     Release  shared  memory  blocks  no  longer being used.\n\
     This command is executed to clean up shared memory  not\n\
     properly  released  when  parallel  data  transfers are\n\
     aborted or interrupted.\n\n", ptr); return(0);
case 'N': sprintf(rtn.msg, "%s -N file-list [email...]\n\n\
    Transfer the files listed in the named file and  delete\n\
    after  the  first successful retrieval.  This option is\n\
    like the -F option except the file names are listed  in\n\
    a  file  rather  than  on the command line.  Wild cards\n\
    cannot be used within the  file,  and  directory  names\n\
    cannot be included.  Secure keywords can be sent to multiple\n\
    users (see the -m option).\n\n", ptr); break;
case 'P': sprintf(rtn.msg, "%s -P file | directory [email...]\n\n\
    Put a file or directory in InterCom  and  delete  after\n\
    the  first  successful  retrieval.  This option is used\n\
    when a file will be retrieved by only one user.\n\n", ptr); break;
case 'R': sprintf(rtn.msg, "%s -R\n\n\
Display URL for NXP registration.\n\n\
             To register for an account go to URL:\n\
https://www.nxp.com/webapp-signup/register\n\n",
ptr); break;
/*
case 'R': sprintf(rtn.msg, "%s -R <email1> [<email2>]\n\n\
Perform InterCom registration.  This command is  used  when\n\
the  user  wants  to  perform registration without executing\n\
another InterCom command.  During registration,  the  user's\n\
login  name,  subnet,  primary  email address, and secondary\n\
email address are added to the InterCom registry.  An  email\n\
message  is  also  sent  to  the  InterCom  administrator to\n\
indicate the user has been registered.  The registry is used\n\
to verify the user is an authorized InterCom user and to map\n\
his login name and subnet to his primary and secondary email\n\
addresses.  If the user is already registered, he can change\n\
his primary and  secondary  email  addresses  by  performing\n\
registration  again.   If  the user does not enter his email\n\
address on the command line, he is prompted for it.   If  he\n\
does  not enter a secondary email address, the primary email\n\
address is used twice.\n\n", ptr); break;
*/
case 'T': sprintf(rtn.msg, "%s -T directory [email...]\n\n\
    Tar a directory tree following symbolic links, transfer\n\
    to  the InterCom repository, and delete after the first\n\
    successful retrieval.  This option  is  different  from\n\
    the  -P  option because symbolic links are not followed\n\
    with the -P option.\n\n", ptr); break;
case 'U': sprintf(rtn.msg, "%s -U keyword\n\n\
    Unlock data to allow data retrieval.   This  option  is\n\
    used  to  unlock  data  previously locked for retrieval\n\
    with the '-L' option.\n\n", ptr); break;
case 'V':  sprintf(rtn.msg, "%s -V\n\n\
          View list of secure keywords received. If the user  has\n\
          write  permission in his current working directory, the\n\
          list  of  keywords  is   saved   in   a   file   called\n\
          'secure_keywords'.   Only  secure  keywords  associated\n\
          with  the  user's  registered   e-mail   address    are\n\
          shown.   Secure   keywords   sent  to  other  users are\n\
          not shown.  This feature is useful when  the  user  has\n\
          lost   or   has  not   received   e-mail  messages with\n\
          secure keywords from  other  users.   this  command  is\n\
          identical to the '-q and -Q' options.\n\n",
ptr); break;
case 'Z': sprintf(rtn.msg, "%s -Z directory file-list [email...]\n\n\
    Zip/package files listed  in  the  named  file  into  a\n\
    directory,  transfer  to  the  InterCom repository, and\n\
    delete after  the  first  successful  retrieval.   This\n\
    option  is like the -K option except the file names are\n\
    specified in a file instead of  on  the  command  line.\n\
    File  names  within  the  file  must be put on separate\n\
    lines.  Wild cards cannot be used within the file,  and\n\
    directory  names  cannot  be  included.   Like  the  -K\n\
    option, the files are bundled together in  a  directory\n\
    during  data  transfer  and  retrieved in the directory\n\
    during data retrieval.  Secure keywords may be sent  to\n\
    multiple users (see the -m option).\n\n", ptr); break;
case 'f': sprintf(rtn.msg, "%s -f file... | directory...\n\n\
    Put files or directories listed  on  the  command  line\n\
    into  the  InterCom  repository.   The files and direc-\n\
    tories are tarred and compressed during  data  transfer\n\
    and  untarred  and  uncompressed during data retrieval.\n\
    The data is put in the receiving user's current working\n\
    directory  as  separate  files  and  directories.  Wild\n\
    cards can be used to specify the   file  and  directory\n\
    names.\n\n", ptr); break;
case 'k': sprintf(rtn.msg, "%s -k directory file...\n\n\
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
          the  file  or  directory  associated  with the keyword.\n\
          Each user is specified by his complete internet  e-mail\n\
          address  i.e.  transbug@nxp.com.  If complete\n\
          internet e-mail addresses are not entered,  the  suffix\n\
          '@nxp.com'  is  added  to  the  end of the e-mail\n\
          address.  The secure keywords sent to users may only be\n\
          used by the user who receives the keyword.  Secure key-\n\
          words can be sent using the -N, -P,  -T,  -Z,  -n,  -p,\n\
          -pl, -pr, -ps, -pt, -t, and -z options.\n\n",
    ptr); break;
case 'n': sprintf(rtn.msg, "%s -n file-list [email...]\n\n\
     Transfer files listed in the named file to the InterCom\n\
     repository.   This  option is like the -f option except\n\
     the file names are listed in a file rather than on  the\n\
     command  line.   File names within the file must be put\n\
     on separate lines.  Blank lines cannot  be  present  in\n\
     the  file.   Wild cards cannot be used within the file,\n\
     and directory names cannot be  included.   Secure  key-\n\
     words may be sent to multiple users (see the -m option).\n\n", ptr); break;
case 'r': sprintf(rtn.msg, "%s -r [releases] | [document]\n\n\
    Display list of commands which can be used to  retrieve\n\
    InterCom releases and documents.  InterCom is available\n\
    on the  following  platforms:   Solaris,   \n\
    Linux, and Windows.\n\n", ptr); break;
case 's': sprintf(rtn.msg, "%s -s keyword | all\n\n\
    Set email notification for one  or  more  files.   When\n\
    email notification is set, the user receives email each\n\
    time one  of his files is retrieved from InterCom.   If\n\
    a  keyword  is used, email notification is set only for\n\
    the associated data.  If the word 'all' is used,  email\n\
    notification  is  set  for all of the user's data.  The\n\
    user may set Email notification only for  data  he  has\n\
    transferred.\n\n", ptr); break;
	case 'S': sprintf(rtn.msg, "%s -s\n\n\
Display Intercom server status.  This command is used by the  \n\
GUI to verify supported servers are active.\n\n",
 ptr); break;
case 't': sprintf(rtn.msg, "%s -t directory [email...]\n\n\
    Tar a  directory  tree  following  symbolic  links  and\n\
    transfer  to  the  InterCom repository.  This option is\n\
    different from the '-p' option because  symbolic  links\n\
    are not followed with the '-P' option.  Secure keywords\n\
    can be sent to multiple users (see the -m option).\n\n", ptr); break;
case 'v': sprintf(rtn.msg, "%s -v\n\n\
    Display program version, server machine,  local\n\
    machine,  InterCom  log  directory,  etc.\n\n", ptr); break;
case 'w':  sprintf(rtn.msg, "%s -w\n\n\
    Ping dropbox machine to verify it can be reached.\n\n", ptr); break;
case 'W':  sprintf(rtn.msg, "%s -W\n\n\
    Silent ping to dropbox machine to verify it can be reached.\n\n",
ptr); break;
case 'z': sprintf(rtn.msg, "%s -z directory file-list [email...]\n\n\
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
case 'X': sprintf(rtn.msg, "%s -X\n\n\
          Encrypt NXP password for storage in the .icomr  file.\n\n",
	ptr); break;
default:
{
	sprintf(rtn.msg, help_err);
	return(HELP_ERR);
} /* End default */
} /* End switch */
    return(SUCCESS);
} /* End trans_help_print */

#endif /* for not NXP */
#endif /* for INTERCOM */
#endif /* For not TRANSAPI */
