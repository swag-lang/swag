struct A {}
impl A
{
    mtd opAffect(x: bool) {}
}

#test
{
    let a: A = true
}
