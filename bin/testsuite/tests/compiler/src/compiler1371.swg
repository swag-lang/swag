using Swag

struct TOTO
{
    x:   func(*TOTO)
}

//#[Swag.PrintBc]
#test
{
    var ptr = cast(const *TypeInfoStruct) #typeof(TOTO)
}
