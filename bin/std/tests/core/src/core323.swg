
using Core

#test
{
    {
        var a = "toto11"
        var b = "toto2"
        var r = Latin1.compareNatural(a, b)
        @assert(r == 1)
    }

    {
        var a = "toto11"
        var b = "toto21"
        var r = Latin1.compareNatural(a, b)
        @assert(r == -1)
    }

    {
        var a = "11"
        var b = "21"
        var r = Latin1.compareNatural(a, b)
        @assert(r == -1)
    }

    {
        var a = "011"
        var b = "021"
        var r = Latin1.compareNatural(a, b)
        @assert(r == -1)
    }

    {
        var a = "11"
        var b = "11"
        var r = Latin1.compareNatural(a, b)
        @assert(r == 0)
    }

    {
        var a = "011"
        var b = "011"
        var r = Latin1.compareNatural(a, b)
        @assert(r == 0)
    }
}