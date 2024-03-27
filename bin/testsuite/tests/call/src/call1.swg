using test_foreign

#test
{
    var x:  tcf.AA
    var itf = cast(tcf.ITcf) x
    @assert(itf.call() == 666)
}
