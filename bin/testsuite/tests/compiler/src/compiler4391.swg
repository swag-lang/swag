#global testerror "Err0591"

struct(T) A {}
impl A
{
    mtd toto(x: const [..] T) {}
}

#test
{
    var x: A's32
    x.toto([1'f32])
}