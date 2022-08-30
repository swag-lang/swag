using test_foreign

#test
{
    x := foufou48("string", 666, 10.0, 20.0, 30.0)
    @assert(x == 555)
}