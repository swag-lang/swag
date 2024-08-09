#global testerror "Err0076"

struct(T) AA
{

}

impl AA
{
    //#[Swag.compileif(@typeof(T) == f32)]
    func toto() where @typeof(T) == f32
    {
    }
}

#test
{
    var x: AA's32
    x.toto()
}