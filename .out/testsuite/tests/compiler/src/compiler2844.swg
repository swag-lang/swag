func print(params: ...)
{
    visit c: params
    {
        @assert(cast(string) c == "this is a string")
    }
}

//#[Swag.PrintBc]
#test
{
    ptr := &print
    ptr("this is a string", "this is a string", "this is a string")
}