using test_foreign

//#[Swag.PrintBc]
#test
{
    tcf_toto30(@(x: s32)
    {
        //@print(x)
        @assert(x == 10)
    })
}