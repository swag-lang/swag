using test_foreign

#test
{
    x := foufou49("string", 666, 10.0, 20.0, 30.0)
    @assert(x.item0 == 444)
    @assert(x.item1 == 555)
}