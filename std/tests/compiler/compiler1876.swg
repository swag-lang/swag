//#[swag.printbc]
#test
{
    {
        x := tcf_toto10()
        @assert(x.x == 10)
        @assert(x.y == 20)
    }

    {
        x := tcf_toto11(666)
        @assert(x.x == 10)
        @assert(x.y == 20)
    }

    {
        x := tcf_toto12(1,2,3,4,5,6)
        @assert(x.x == 10)
        @assert(x.y == 20)
    }
}