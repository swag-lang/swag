#global testerror

struct(T) AA
{

}

impl AA
{
    #[swag.compileif(@typeof(T) == f32)]
    func toto()
    {
    }
}

#test
{
    var x: AA's32
    x.toto()
}