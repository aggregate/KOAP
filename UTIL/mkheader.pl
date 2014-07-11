#!/usr/bin/perl
use strict;

if (@ARGV != 1)
{
	print "Usage: mkheader.pl [OpenCL kernel file name]\n";
	die;
}
my $filename = $ARGV[0];
		
my @splitname = split(/\./, $filename);

open(OUTFILE, ">$splitname[0].h");
print OUTFILE "char kernelstr[] = \n";

open(INFILE, "<$filename");
my $line;
while (defined($line = <INFILE>))
{
	print OUTFILE "\"";
	$line =~ s/\n/\\n\"\n/;
	print OUTFILE $line;
}

close INFILE;

print OUTFILE ";";
close OUTFILE;

