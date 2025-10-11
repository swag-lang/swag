using test_foreign

//#[Swag.PrintBc]
#test
{
    toto30(func(x: s32)
    {
        //@print(x)
        @assert(x == 10)
    })
}
