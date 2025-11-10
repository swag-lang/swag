using Swag

func panic(msg: string, loc: Swag.SourceCodeLocation) {}

//#[PrintBc]
#test
{
    let cxt = @getcontext()
    cxt.panic = &panic
    defer cxt.panic = null
}
