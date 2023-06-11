func set(ptr: ^u8)
{
    ptr[0] = 0
}

//#[Swag.PrintBc]
#test
{
    cxt := @getcontext()
    ptr := &cxt.errors[0].msgBuf[0]
    set(ptr)
}