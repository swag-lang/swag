#global testerror "Err0745"

struct(T) Arr {}
impl Arr
{
    //mtd opAffect(x: T) {}
    mtd opAffect(x: const [..] T) {}
}

#test
{
    var a: Arr's32 = 1
}
