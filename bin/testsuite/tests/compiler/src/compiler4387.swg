#global testerror "Err0081"
struct(T) Arr {}
impl Arr
{
    //mtd opAffect(x: T) {}
    mtd opAffect(x: const [..] T) {}
}

#test
{
    var a3: Arr = [1's8]
}