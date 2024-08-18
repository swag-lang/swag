func toto(x: s32)
{
    @print(x)
}

//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
#test
{
    loop i in 0
    {
        toto(i)
    }
}