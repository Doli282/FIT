#!/usr/bin/perl

use strict;
use warnings;

# default greeting
my $greeting = "Hi";

#print "Arguments $#ARGV\n";

# if argument is present, change the greeting
if($#ARGV > -1){
  $greeting = $ARGV[0];
}

# uses ternar operator
# $greeting = ($#ARGV > -1) ? $ARGV[0] : "default greeting";

# print output
print "$greeting".q( )."$ENV{USER}\n";
