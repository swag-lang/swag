func toto(x: s32)
{
    @print(x)
}

//#[Swag.PrintBc]
//#[Swag.Optimize(.All, false)]
#test
{
    for i in 0
    {
        toto(i)
    }
}
