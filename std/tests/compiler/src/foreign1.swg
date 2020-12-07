#module "tests_call_compiler_foreign"
#import "tests_compiler_foreign"
using tests_compiler_foreign

#test
{
    var x: tcf.AA
    itf := cast(tcf.ITcf) x
    @assert(itf.call() == 666)
}