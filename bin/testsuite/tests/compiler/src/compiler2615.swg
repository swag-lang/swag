#global testerror "Err0004"

struct(T) AA
{

}

impl AA
{
    //#[Swag.compileif(@typeof(T) == f32)]
    func toto() #validif @typeof(T) == f32
    {
    }
}

#test
{
    var x: AA's32
    x.toto()
}