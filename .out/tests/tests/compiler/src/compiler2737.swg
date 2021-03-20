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

#[swag.inline]
func toto()->s32 throw
{
    return try tata()
}

//#[swag.printbc]
func titi()->s32 throw
{
    var x: AA
    return try toto()
}

//#[swag.printbc]
#test
{
    a := catch titi()
    @assert(G == 1)
}