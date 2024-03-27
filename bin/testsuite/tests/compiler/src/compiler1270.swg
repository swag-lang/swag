enum A : u32
{
    B
}

struct T
{
}

impl T
{
    #[Swag.Overload]
    func opAffect(using self, val: u32)
    {
    }

    #[Swag.Overload]
    func opAffect(using self, val: A)
    {
    }
}

#test
{
    var x: T = A.B
}
