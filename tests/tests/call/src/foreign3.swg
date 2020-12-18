#import "tests_foreign"
using tests_foreign

//#[swag.printbc]
#test
{
    tcf_toto16(456,3.14)

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

    @assert(tcf_toto13(1,"toto") == "toto")
    @assert(tcf_toto14(1,2,"toto") == "toto")
    @assert(tcf_toto15(1,2,3,"toto") == "toto")
}