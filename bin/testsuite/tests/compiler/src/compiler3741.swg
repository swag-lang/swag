#global testerror "Err0016"
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