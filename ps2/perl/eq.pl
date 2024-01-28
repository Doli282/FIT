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

my $match;

BEGIN {
  if($#ARGV < 0){
    print STDERR "wrong input -> 1st argument is matching word\n";
    exit;
  }
  $match = $ARGV[0];
  shift @ARGV;
  my $test = $match;
  unless($test =~ s/^[[:alpha:]][[:alnum:]]*$/1/){
    print STDERR "wrong input -> 1st argument is matching word\n";
    exit;
  }
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
my $filename;

#no files in arguments -> read STDIN
if($#ARGV < 0){
  while ($line = <STDIN>){
    chomp $line;
    $line =~ s/:.*$//;
    if($line eq $match){
      print $line."\n";
      $cnt++;
    }
  }
}

#foreach my $filename (@ARGV) {
while(@ARGV){
  $filename = $ARGV[0];
  shift @ARGV;
  unless(-f $filename) { print STDERR $filename." is not file\n"; next;}
  if(!open($fd, "<", $filename)) { print STDERR "could not open ".$filename."\n"; next;}
  
  #print first column = username = in all rows
  while ($line = <$fd>){ 
    chomp $line;
    $line =~ s/:.*$//;
    if($line eq $match){
      print $line."\n";
      $cnt++;
    }
  }
 
  close($fd) or print STDERR "could not close ".$filename."\n";
};

#print footer
END {
  # print "=" horizontal delimiter
  &function;
  
  #print number of printed lines
  print "Footer of the list: lines=".$cnt."\n";
}
