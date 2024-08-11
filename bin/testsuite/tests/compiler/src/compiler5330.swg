#global testerror "Err0599"
struct(T) Arr {}
impl Arr
{
    mtd toto() {}
}

#test
{
    let a: Arr's32
    a.toto()
}