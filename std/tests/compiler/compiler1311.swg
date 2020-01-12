//#[swag.printbc]
#test
{
    cpt := 0
    
    label outer loop x: 10
    {
        cpt += 1   
        loop 10
        {
            continue outer
        }   

        cpt += 1        
    }

	@assert(cpt == 10)
}