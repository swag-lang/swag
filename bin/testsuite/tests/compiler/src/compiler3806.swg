struct A {}

impl A
{
    mtd(op: string) opAssign(x: string) {}

    #[Swag.Implicit, Swag.Inline]
    mtd opCast()->string
    {
        return ""
    }
}

#test
{
    var x: A
    var y: A
    x += y
}
