using Swag

func panic(msg: string, loc: const ref Swag.CompilerSourceLocation)
{
}

//#[PrintBc]
#test
{
    cxt := @getcontext()
    cxt.panic = &panic
    defer cxt.panic = null
}