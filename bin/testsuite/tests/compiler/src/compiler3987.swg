using Swag

func panic(msg: string, loc: Swag.SourceCodeLocation)
{
}

//#[PrintBc]
#test
{
    var cxt = @getcontext()
    cxt.panic = &panic
    defer cxt.panic = null
}