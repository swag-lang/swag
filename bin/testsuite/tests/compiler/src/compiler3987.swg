using Swag

func panic(msg: string, loc: const ref Swag.SourceCodeLocation)
{
}

//#[PrintBc]
#test
{
    cxt := @getcontext()
    cxt.panic = &panic
    defer cxt.panic = null
}