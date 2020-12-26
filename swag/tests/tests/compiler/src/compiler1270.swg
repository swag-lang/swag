enum A : u32
{
    B
}

struct T
{
}

impl T
{
    func opAffect(using self, val: u32)
    {
    } 
    
    func opAffect(using self, val: A)
    {
    }     
}
    
#test
{
    var x: T = A.B
}