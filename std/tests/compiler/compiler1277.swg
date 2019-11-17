macro toto(x: code)
{
}
    
#[swag.printbc]
#test
{
    b := 0
    toto(#code{b = 666;})
}