##perl -MConfig -e 'print join(" ", @Config{qw(ccflags optimize cccdlflags)},"-I$Config{archlib}/CORE")'

  
swig -perl5 example.i
 
CFLAGS="
-D_REENTRANT -D_GNU_SOURCE -DDEBIAN -fstack-protector -fno-strict-aliasing -pipe -I/usr/local/include -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O2 -g -fPIC -I/usr/lib/perl/5.14/CORE"

#CFLAGS="-D_REENTRANT -D_GNU_SOURCE -fno-strict-aliasing -pipe -Wdeclaration-after-statement -I/usr/local/include -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -I/usr/include/gdbm -O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector --param=ssp-buffer-size=4 -m64 -mtune=generic -fPIC -I/usr/lib/perl/5.14/CORE"

 gcc -c $CFLAGS example.c example_wrap.c

 gcc `perl -MConfig -e 'print $Config{lddlflags}'` example.o example_wrap.o -o example.so
 
perl test.pl
 
 exit
 
 cat <<EOF | perl
 use example;
 use Data::Dumper;
 print \$example::My_variable,"\n";
 print example::fact(5),"\n";
 print example::get_time(),"\n";
 
  my \$msg =  "carlos";
  
  print "ref=$msg\n";
 
  my (\$ret) = example::icom_put_compress_init(1, \$msg);	                        
   
EOF 
