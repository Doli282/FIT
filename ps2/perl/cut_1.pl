#!/usr/bin/perl -anF:

use strict;
use warnings;

sub function{
# print "=" horizontal delimiter
  my $fill = "=";
  if(defined($ENV{COLUMNS})){ # if COLUMNS is exported
    $fill x= $ENV{COLUMNS};
  } else { #default value
    $fill x= 80;
  }
  print $fill."\n";
}

#print header
BEGIN {
  #print names of input files or STDIN if there arent any
  print "Header of the list: usernames in";
  if($#ARGV > -1){
    print " ".$_ foreach @ARGV;
  } else {
    print " STDIN";
  }
  print "\n";
  
  # print "=" horizontal delimiter
  &function;
}

#print first column = username = in all rows
print $F[0]."\n";

#print footer
END {
  # print "=" horizontal delimiter
  &function;
  
  #print number of printed lines
  print "Footer of the list: lines=".$.."\n";
}
