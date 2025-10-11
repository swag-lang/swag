#test
{
    enum Enum { A, B, C }

    var a = 0
    foreach it, idx in #typeof(Enum).values
    {
        a += 1
        switch idx
        {
        case 0:
            @assert(it.name == "A")
        case 1:
            @assert(it.name == "B")
        case 2:
            @assert(it.name == "C")
        }
    }

    @assert(a == 3)
}
