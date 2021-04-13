func print(params: ...)
{
    visit c: params
    {
        @assert(cast(string) c == "this is a string")
    }
}

//#[swag.printbc]
#test
{
    ptr := &print
    ptr("this is a string", "this is a string", "this is a string")
}