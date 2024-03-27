#global testerror "Err0114"
struct(T) Toto
{
}

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
