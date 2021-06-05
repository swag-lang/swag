func set(ptr: *u8)
{
    ptr[0] = 0
}

//#[Swag.printbc]
#test
{
    cxt := @getcontext()
    ptr := &cxt.errorMsg[0]
    set(ptr)
}