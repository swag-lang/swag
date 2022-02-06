
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
}