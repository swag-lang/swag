using Core

#test
{
    {
        let a = "toto11"
        let b = "toto2"
        let r = Latin1.compareNatural(a, b)
        @assert(r == 1)
    }

    {
        let a = "toto11"
        let b = "toto21"
        let r = Latin1.compareNatural(a, b)
        @assert(r == -1)
    }

    {
        let a = "11"
        let b = "21"
        let r = Latin1.compareNatural(a, b)
        @assert(r == -1)
    }

    {
        let a = "011"
        let b = "021"
        let r = Latin1.compareNatural(a, b)
        @assert(r == -1)
    }

    {
        let a = "11"
        let b = "11"
        let r = Latin1.compareNatural(a, b)
        @assert(r == 0)
    }

    {
        let a = "011"
        let b = "011"
        let r = Latin1.compareNatural(a, b)
        @assert(r == 0)
    }
}
