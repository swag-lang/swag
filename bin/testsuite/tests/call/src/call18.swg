using test_foreign

//#[Swag.PrintBc]
#test
{
    @assert(tcf_float() >= 3.13 and tcf_float() <= 3.15)
    @assert(tcf_double() >= 3.13 and tcf_double() <= 3.15)
}
