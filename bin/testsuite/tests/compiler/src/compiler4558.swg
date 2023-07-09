struct A {}
impl A
{
    mtd(op: string) opUnary()->A
    {
        return {}
    }
}

#test
{
    var a: A
    var b: ref A = &a
    var c = !b
}