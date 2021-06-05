alias T = func()->void

//#[Swag.printbc]
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