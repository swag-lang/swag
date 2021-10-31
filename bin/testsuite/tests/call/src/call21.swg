using test_foreign

//#[Swag.PrintBc]
#test
{
    (a, b) := tcf_toto26("127")
    @assert(a == 1)
    @assert(b == 2)
}