using swag

struct TOTO
{
}

interface Itf
{
    print: func(self)
}

impl Itf for TOTO
{
    func print(self)
    {
    }
}

//#[swag.printbc]
#test
{   
    ptr := cast(const *TypeInfoStruct) @typeof(TOTO)
    @assert(@countof(ptr.interfaces) == 1)
}