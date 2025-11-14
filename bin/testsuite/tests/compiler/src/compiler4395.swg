func toto(x: typeinfo) {}

//#[Swag.PrintBc]
#test
{
    toto(#type [2] s32)
}
