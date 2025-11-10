//#global skip
interface I
{
    mtd toto();
}

struct X {}

impl I for X
{
    mtd impl toto() {}
}

#test
{
    var x: X
    let i  = cast(I) x
    switch i
    {
    case X:
        break
    default:
        //@print(@kindof(i).name)
        @assert(false)
    }
}
