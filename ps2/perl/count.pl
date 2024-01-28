#!/usr/bin/perl

use strict;
use warnings;

if($#ARGV < 0){
  print STDERR "missing armunets\n";
  exit 1;
}

my $sum = 0;
my $cnt = 0;

foreach my $arg (@ARGV){
	$cnt = 0;
  while($arg =~ m/O/g){
  	$cnt++;
  }
  $sum += $cnt;
  print "v $arg je ".$cnt."x O\n";
}
print "celkem predano ".$sum." O\n";
