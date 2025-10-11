func print(params: ...)
{
    foreach c in params
    {
        @assert(cast(string) c == "this is a string")
    }
}

//#[Swag.PrintBc]
#test
{
    let ptr = &print
    ptr("this is a string", "this is a string", "this is a string")
}
