#global testerror

struct(T) AA
{

}

impl AA
{
    //#[swag.compileif(@typeof(T) == f32)]
    func toto() #selectif @typeof(T) == f32
    {
    }
}

#test
{
    var x: AA's32
    x.toto()
}