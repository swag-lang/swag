#global testerror "Err0573"

struct A {x: s32}
impl A {
    mtd opDrop(){}
}
func toto() throw
{
    throw A{42}
}

#test
{
    catch toto()
}