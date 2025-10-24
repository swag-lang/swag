#global testerror "Err0090"
#global #if @compiler.getBuilfCfg().byteCodeOptimizeLevel > 0

#[Swag.Safety(.All, .Math, false)]
func toto(y: f32)
{
    var z = @sqrt(y)
    @print(z)
}

#[Swag.Safety(.All, .Math, false)]
#test
{
    var y = -1.0'f64
    var x = @sqrt(y)
    @print(x)
    //toto(-1.0)
}
