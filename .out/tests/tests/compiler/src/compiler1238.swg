alias T = func()->void

//#[swag.printbc]
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