#! /usr/bin/perl
use 5.010;

my $ifname = $ARGV[0];
my $ofname = $ifname . "_hex";

my $flag = 0;

open IN, "$ifname";
open OUT, "> $ofname";

printf OUT "memory_initialization_radix = 16;\n";
printf OUT "memory_initialization_vector = ";

<>;
<>;
<>;
<>;
<>;

while(<>)
{
    chomp;
    if(s/([\da-f]+:)//)
    {
        if(/([\da-f]+)/)
        {
            if($flag == 0)
            {
                printf OUT "$1";
                $flag = 1;
            }
            else
            {
                printf OUT ",\n$1";
            }
        }
    }
}

printf OUT ";";
