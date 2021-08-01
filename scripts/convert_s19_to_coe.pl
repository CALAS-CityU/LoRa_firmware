#Perl script is used to convert the S19 exe file to COE files for 
#the initialization for Pulpino instruction RAM and data RAM.
#NOTE: This script only applies for the format in which the start address
#of each line is 4-bype aligned.

#! /usr/bin/perl
use 5.010;

my $ifname = $ARGV[0];
my $ofname1 = $ifname . "_inst";
my $ofname2 = $ifname . "_data";

my $flag = 0;

open IN, "$ifname";

open OUT, "> $ofname1";
printf OUT "memory_initialization_radix = 16;\n";
printf OUT "memory_initialization_vector = ";

my $file_sel = 0;
my $line = 0;
my $addr_cur = 0;
my $addr_start = 0;
my $temp_data;
my $byte_cnt = 0;

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
        $line = 0;
        $byte_cnt = 0;
        printf OUT ";";
        close OUT;
        open OUT, "> $ofname2";
        printf OUT "memory_initialization_radix = 16;\n";
        printf OUT "memory_initialization_vector = ";
    }

    while($addr_cur < $addr_start) #align data for the start address in a new line
    {
        while($byte_cnt < 4)
        {
            $temp_data = "00" . $temp_data;
            $addr_cur ++;
            $byte_cnt ++;
        }
        if($line == 0)
        {
            printf OUT "$temp_data";
        }
        else
        {
            printf OUT ",\n$temp_data";
        }
        $byte_cnt = 0;
        $temp_data = "";
    }

    s/[\dA-F]{2}$//;  #chop off the parity byte
    $temp_data = "";
    while(s/([\dA-F]{2})//) #write data to COE file
    {
        $temp_data = $1 . $temp_data;
        $byte_cnt ++;
        $addr_cur ++;
        if($byte_cnt == 4)
        {
            if($line == 0)
            {
                printf OUT "$temp_data";
            }
            else
            {
                printf OUT ",\n$temp_data";
            }
            $temp_data = "";
            $byte_cnt = 0;
            $line ++;
        }
    }
}

printf OUT ";";

if($file_sel == 0) #no initial data for data ram, write an empty coe
{
    close OUT;
    open OUT, "> $ofname2";
    printf OUT "memory_initialization_radix = 16;\n";
    printf OUT "memory_initialization_vector = ;";
}

close IN;
close OUT;

