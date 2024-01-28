#!/usr/bin/perl

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

my $cnt = 0;
my $line;
my $fd;

{
if($#ARGV < 0){
  while ($line = <STDIN>){ 
    $line =~ s/:.*$//;
    print $line;
    $cnt++;
  }
  last;
}

foreach my $filename (@ARGV) {
  unless(-f $filename) { print STDERR $filename." is not file\n"; next;}
  if(!open($fd, "<", $filename)) { print STDERR "could not open ".$filename."\n"; next;}
  
  #print first column = username = in all rows
  while ($line = <$fd>){ 
    $line =~ s/:.*$//;
    print $line;
    $cnt++;
  }
 
  close($fd) or print STDERR "could not close ".$filename."\n";
}
}


#print $F[0]."\n";

#print footer
END {
  # print "=" horizontal delimiter
  &function;
  
  #print number of printed lines
  print "Footer of the list: lines=".$cnt."\n";
}
