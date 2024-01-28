#!/usr/bin/perl

use strict;
use warnings;

#autoflush
$| = 1;



# no argument = imediate output
if ($#ARGV < 0){
  print "BOOM!\n";
  exit 0;
}

#more arguments = warning
if ($#ARGV > 0){
  print "boom.pl: only the first argument counts\n";
}

# input control
unless ($ARGV[0] =~ /^[0123456789]+$/){
  die "boom.pl: Invalid argument\n";
}

my $input = $ARGV[0];





# countdown per second
for (my $countdown = $input; $countdown > 0; $countdown--) {
  print $countdown." ";
  sleep(1);
}




# print BOOM!
print "B";
if ($input > 2){
  for (my $i = $input; $i > 0; $i--){
    print "O";
  }
} else {
  print "OO";
}
print "M!\n";

exit 0;
