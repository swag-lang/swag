struct AA
{
    x: u64
}

var G = 0

impl AA
{
    func opDrop(using self)
    {
        G = 1
    }
}

#[swag.inline]
func toto()->AA  
{ 
    return AA{}; 
}

//#[swag.printbc]
#test
{
    {
        toto()
    }
    @assert(G == 1)
}