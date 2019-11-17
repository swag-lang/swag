//#[swag.printbc]
#test
{
    a := 666
    if a == 666
    {
        func toto()
        {
            a := 1
        }
        
        toto()
    }
    
    @assert(a == 666)
}