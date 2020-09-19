#module "compiler.foreign"
#import "tests.compiler.foreign"

#test
{
    var x: tcf.AA
    itf := cast(tcf.ITcf) x
    @assert(itf.call() == 666)
}