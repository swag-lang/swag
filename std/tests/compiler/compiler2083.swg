struct AA
{
    x: u64
}

impl AA
{
    func opDrop(using self)
    {
        @print("drop\n")
    }
}

#[swag.inline]
func toto()->AA  
{ 
    return AA{}; 
}

#[swag.printbc]
#test
{
    toto()
}