func(T) delete(addr: *T)
{
    @assert(dref addr == 666)
}

//#[Swag.PrintBc]
#test
{
    var x = 666
    delete's32(&x)
}
