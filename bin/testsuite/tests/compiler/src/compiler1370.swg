using Swag

struct TOTO
{
}

interface Itf
{
    func print(self)
}

impl Itf for TOTO
{
    func impl print(self)
    {
    }
}

//#[Swag.PrintBc]
#test
{
    var ptr = cast(const *TypeInfoStruct) @typeof(TOTO)
    @assert(@countof(ptr.interfaces) == 1)
    @assert(ptr.interfaces[0].name == "__compiler1370.Itf")
}