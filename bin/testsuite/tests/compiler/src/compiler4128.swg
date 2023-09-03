#global testerror "Err0112"
#global if @compiler().getBuilfCfg().byteCodeOptimizeLevel > 0

#[Swag.Safety("math", false)]
func toto(y: f32)
{
    var z = @sqrt(y)
    @print(z)
}

#[Swag.Safety("math", false)]
#test
{
    var y = -1.0'f64
    var x = @sqrt(y)
    @print(x)
    //toto(-1.0)
}