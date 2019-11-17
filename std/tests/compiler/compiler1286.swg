//#[swag.printbc]
#test
{
    a := 666
    if a == 666
    {
        macro toto()
        {
            a = 1
        }
        
        toto()
    }
    
    @assert(a == 1)
}