#global skip
func set(ptr: [*] u8)
{
    ptr[0] = 0
}

//#[Swag.PrintBc]
#test
{
    let cxt = @getcontext()
    let ptr = &cxt.errors[0].buf[0]
    set(ptr)
}