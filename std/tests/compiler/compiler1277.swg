macro toto(x: code)
{
    #insert x
    #insert x
    #insert x
}
    
//#[swag.printbc]
#test
{
    b := 0
    toto(#code{b += 1;})
    @assert(b == 3)
}