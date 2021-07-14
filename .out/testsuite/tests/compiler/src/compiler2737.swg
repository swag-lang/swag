var G = 0

struct AA
{

}

impl AA
{
    func opDrop(using self)
    {
        G += 1
    }
}

func tata()->s32 throw
{
    throw "toto"
    return 0
}

#[Swag.inline]
func toto()->s32 throw
{
    return try tata()
}

//#[Swag.PrintBc]
func titi()->s32 throw
{
    var x: AA
    return try toto()
}

//#[Swag.PrintBc]
#test
{
    a := catch titi()
    @assert(G == 1)
}