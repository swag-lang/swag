enum A: u32
{
    B
}

struct T {}

impl T
{
    #[Swag.Overload]
    func opAffect(me, val: u32) {}

    #[Swag.Overload]
    func opAffect(me, val: A) {}
}

#test
{
    var x: T = A.B
}
