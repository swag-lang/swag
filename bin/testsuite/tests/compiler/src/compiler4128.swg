#global testerror
#global if @compiler().getBuilfCfg().byteCodeOptimizeLevel > 0

#[Swag.Safety("math", false)]
func toto(y: f32)
{
    z := @sqrt(y)
    @print(z)
}

#[Swag.Safety("math", false)]
#test
{
    y := -1.0'f64
    x := @sqrt(y)
    @print(x)
    //toto(-1.0)
}