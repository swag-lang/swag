#global testerror "Err0161"

func toto(x: s32)
{
}

#test
{
    var y = &toto(1)
    //var x = @kindof(@getcontext().allocator)
}
