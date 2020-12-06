#module "tests_call_compiler_foreign"
#import "tests_compiler_foreign"
using tests_compiler_foreign

//using tcf
#test
{
    @assert(tcf_toto9(1,2,3,4,5,6) == "toto")
    @assert(tcf_toto8(1,2,3,4,5,6) == 21)
    @assert(tcf_toto7(1,2,3,4) == 10)
    @assert(tcf_toto6("666") == "666")
    @assert(tcf_toto5(666) == 666)
    @assert(tcf_toto4() == "toto")
    tcf_toto3(1,2,3,4,5,6)
    tcf_toto()
    tcf_toto1(64)
    tcf_toto2(1,2,3,4)
}