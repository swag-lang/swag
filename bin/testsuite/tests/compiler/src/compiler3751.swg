#global testerror

func toto(x: s32)
{
}

#test
{
    y := &toto(1)
    //x := @kindof(@getcontext().allocator)
}
