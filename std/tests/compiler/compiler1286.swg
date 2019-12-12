//#[swag.printbc]
#test
{
    a := 666
    if a == 666
    {
        #[swag.mixin]
        func toto()
        {
            a = 1
        }
        
        toto()
    }
    
    @assert(a == 1)
}