using Swag

struct TOTO
{
    x: func(*TOTO)
}

//#[Swag.PrintBc]
#test
{
    ptr := cast(const *TypeInfoStruct) @typeof(TOTO)
}