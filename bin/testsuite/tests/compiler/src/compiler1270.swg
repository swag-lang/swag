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
    func opAffect(using me, val: u32)
    {
    }

    #[Swag.Overload]
    func opAffect(using me, val: A)
    {
    }
}

#test
{
    var x: T = A.B
}