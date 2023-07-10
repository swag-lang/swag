func titi(ptr: ...)
{
    let ptr1 = cast(*Swag.SourceCodeLocation) @dataof(ptr[0])
    @assert(ptr1.lineStart == 9)
}

//#[Swag.PrintBc]
#test
{
    titi(#location)
    //var x = toto("y")
}