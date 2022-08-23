#global skip
using test_foreign

#test
{
    f := foufou45()
    x := f(666)
    @assert(x == 666)
}