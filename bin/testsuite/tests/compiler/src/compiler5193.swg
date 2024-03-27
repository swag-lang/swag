/*#global testerror "Err0379"

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
}*/