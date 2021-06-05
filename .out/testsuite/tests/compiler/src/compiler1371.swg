using Swag

struct TOTO
{
    x: func(*TOTO)
}

//#[Swag.printbc]
#test
{
    ptr := cast(const *TypeInfoStruct) @typeof(TOTO)
}