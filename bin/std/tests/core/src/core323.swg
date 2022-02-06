
using Core

#test
{
    {
        a := "toto11"
        b := "toto2"
        r := Latin1.compareNatural(a, b)
        @assert(r == 1)
    }

    {
        a := "toto11"
        b := "toto21"
        r := Latin1.compareNatural(a, b)
        @assert(r == -1)
    }

    {
        a := "11"
        b := "21"
        r := Latin1.compareNatural(a, b)
        @assert(r == -1)
    }

    {
        a := "011"
        b := "021"
        r := Latin1.compareNatural(a, b)
        @assert(r == -1)
    }

    {
        a := "11"
        b := "11"
        r := Latin1.compareNatural(a, b)
        @assert(r == 0)
    }

    {
        a := "011"
        b := "011"
        r := Latin1.compareNatural(a, b)
        @assert(r == 0)
    }
}