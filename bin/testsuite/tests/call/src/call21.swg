using test_foreign

//#[Swag.PrintBc]
#test
{
    var (a, b) = tcf_toto26("127")
    @assert(a == 1)
    @assert(b == 2)
}
