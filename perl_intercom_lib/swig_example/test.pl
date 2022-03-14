use example;
 use Data::Dumper;
 
#  print $example::My_variable,"\n";
#  print example::fact(5),"\n";
 my $time  = example::get_time();
 
 
 print "time=$time\n";
 
  my $msg =  "carlos";
  
  my $ref = undef;
  
 
  my ($out)= example::icom_put_compress_init(1, $msg);
  
  printf  ("out =  %x  %s\n", $out, $out);
#   
#   my $test = example::printmsg();
#   
#   print "test=$test";