#Perl script is used to convert the S19 exe file to the memory file for the
#on-chip programmer to program the Pulpino core.

#! /usr/bin/perl
use 5.010;

my $ifname = $ARGV[0];
my $tfname1 = $ifname . "_temp1";
my $tfname2 = $ifname . "_temp2";
my $ofname = $ifname . "_mem";

open IN, "$ifname";

open OUT, "> $tfname1";

my $file_sel = 0;
my $addr_cur = 0;
my $addr_start = 0;
my $ilen = 0;
my $dlen = 0;

my $ilen_hex_s = "";
my $dlen_hex_s = "";

while(<>)
{
    chomp;
    s/S([\dA-F])[\dA-F]{2}//; #get the S type and the length
    if($1 == '1')
    {
        s/([\dA-F]{4})//;
        $addr_start = hex("0x" . $1);
    }
    elsif($1 == '2')
    {
        s/([\dA-F]{6})//;
        $addr_start = hex("0x" . $1);
    }
    elsif($1 == '3')
    {
        s/([\dA-F]{8})//;
        $addr_start = hex("0x" . $1);
    }
    else
    {
        next;
    }

    if($addr_start >= 0x100000 && $file_sel == 0) #switch to data ram part
    {
        $addr_cur = 0x100000;
        $file_sel = 1;
        close OUT;
        open OUT, "> $tfname2";
    }

    while($addr_cur < $addr_start) #align data for the start address in a new line
    {
        printf OUT "00";
        $addr_cur ++;
        if($file_sel == 0)
        {
            $ilen ++;
        }
        else
        {
            $dlen ++;
        }
    }

    s/[\dA-F]{2}$//;  #chop off the parity byte
    while(s/([\dA-F]{2})//) #write data to file
    {
        printf OUT "$1";
        $addr_cur ++;
        if($file_sel == 0)
        {
            $ilen ++;
        }
        else
        {
            $dlen ++;
        }
    }
}

close IN;
close OUT;

open OUT, "> $ofname";

#memory file header 20210701
print OUT "20210701";

#instruction memory size
$ilen_hex_s = sprintf("%08X", $ilen);
print OUT "$ilen_hex_s";

#data memory size
$dlen_hex_s = sprintf("%08X", $dlen);
print OUT "$dlen_hex_s";

close OUT;

system "cat $tfname1 >> $ofname";
system "cat $tfname2 >> $ofname";

system "rm -f $tfname1";
system "rm -f $tfname2";

