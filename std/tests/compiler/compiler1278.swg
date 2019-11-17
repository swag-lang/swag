macro toto(x: code)
{
    loop 10
    {
        #insert x
    }
}
    
//#[swag.printbc]
#test
{
    b := 0
    toto(#code{b += 1;})
    @assert(b == 10)
}