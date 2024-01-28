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


my $fd;
# prepare source file
  if($#ARGV < 0){
    print STDERR "match.pl: arguments missing\n";
    exit 1;
  }
  my $filename=$ARGV[0];
  unless(-f $filename) { print STDERR $filename." is not file\n"; exit 1;}
  shift @ARGV;

# check UID in arguments
  if($#ARGV < 0){
    print STDERR "match.pl: UID in arguments missing\n";
    exit 1;
  }
  
#print header  
  #print names of input files or STDIN if there arent any
  print "Header of the list: usernames with UIDs in ".$filename."\n";

  # print "=" horizontal delimiter
  &function;


my $cnt = 0;
my $line;

foreach my $UID (@ARGV) {
  if($UID !~ m/^\d+$/){print STDERR "match.pl: invalid UID '".$UID."'\n"; next;}
  if(!open($fd, "<", $filename)) { print STDERR "could not open ".$filename."\n"; exit 1;}
  my $flag = 0;
  #print first column = username = in all rows
  while ($line = <$fd>){
    # match given UID
    if($line =~ m/^ [^:]* : [^:]* : $UID : .*$/x){
      # select username and UID
      $line =~ s/^ ([^:]*) : [^:]* : ($UID) : .*$/$1:$2/x;
      print $line;
      $cnt++;
      $flag = 1;
    }
  }
  if($flag == 0) { print "$UID was not found\n";}
  close($fd) or print STDERR "could not close ".$filename."\n";  
}


#print footer
  # print "=" horizontal delimiter
  &function;
  
  #print number of printed lines
  print "Footer of the list: lines=".$cnt."\n";

