#global testerror "Err0017"
struct(T) Toto {}

impl Toto
{
    mtd(T) titi()
    {
    }
}

#test
{
    var toto: Toto's32
    toto.titi'bool()
}