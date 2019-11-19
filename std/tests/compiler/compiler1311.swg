#[swag.printbc]
#test
{
    cpt := 0
    outer: loop x: 10
    {
        cpt += 1   
        loop 20
        {
            continue outer
        }   

        cpt += 1        
    }

	@print(cast(s64) cpt)
}