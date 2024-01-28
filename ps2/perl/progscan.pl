#!/usr/bin/perl

use strict;
use warnings;

# kontrola poctu argumentu
if($#ARGV != 1){
  exit 2;
}

# kontrola obsahu argumentu
foreach my $arg (@ARGV){
  # kontrola neprazdnosti argumentu
  if(defined $arg && ! length $arg){
    exit 2;
  }

  # kontrola metaznaku
  if($arg =~ m|[\.\+\?\*\{\)\(\[\^\$\\\|]|){
    exit 3;
  }
}


# hledani shody
if($ARGV[0] !~ m/$ARGV[1]/ ){
  # pokud neni shoda nalezena vypis informacni hlasku a ukonci program
  print "$ARGV[1] not found\n";
  exit 1;
}

# shoda nalezena -> vypis text
print "$ARGV[0]\n";

# hledani pozic
# retezec s ukazateli na pozice nalezenych vzoru
my $ptr = $ARGV[0];
# rozsireni nahrazovaciho retezce na velikost vzoru minus jedna (minus ^ na zacatku)
my $fill = "";
for(my $i = 1; $i < length $ARGV[1]; $i++){
  # pokud je ve vzoru \n nahradi ulozi na tuto pozici \n jinak nahradi znak za mezeru
  if($ARGV[1] =~ m/.{$i}\n.*/){
    $fill = "$fill"."\n";
  } else {
  $fill = "$fill"." ";
  }
}

# nahrad vsechny nalezene vzory za ukazatel dopleneny mezerami
$ptr =~ s/$ARGV[1]/\^$fill/g;
# nahrad vsechny zbyvajici znaky (krome mezer) za mezery
$ptr =~ s/[^\^\s]/ /g;
# zbav se mezer na konci
$ptr =~ s/(.*\^).*/$1/gs;
# vypis ukazatele
print "$ptr\n";
exit 0;
