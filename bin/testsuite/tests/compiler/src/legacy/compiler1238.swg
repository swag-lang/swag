alias T = func()->void

//#[Swag.PrintBc]
#test
{
    var x: T
    if(!x)
    {
    }

    if(x)
    {
        @assert(false)
    }
}