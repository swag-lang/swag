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
    let b: ref A = &a
    let c = !b
}