struct Arr {}

impl Arr
{
    mtd(V) toto(x: V) {}
}

#test
{
    var a: Arr
    a.toto({1, 1})
}
