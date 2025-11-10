var G = 0

struct ErrToto {}

struct AA {}

impl AA
{
    func opDrop(me)
    {
        G += 1
    }
}

func tata()->s32 throw
{
    throw ErrToto{}
    return 0
}

#[Swag.Inline]
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
    var a = catch titi()
    @assert(G == 1)
}
