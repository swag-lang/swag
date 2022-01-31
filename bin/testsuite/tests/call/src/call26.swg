using test_foreign

//#[Swag.PrintBc]
#test
{
    tcf_toto30(@(x: s32)
    {
        @assert(x == 10)
    })
}