alias Vec4 = [8] f32

//#[Swag.PrintBc]
#[Swag.NoInline]
func toto(x: Vec4, t: Vec4)
{
    var y: Vec4 = [1,2,3,4,1,2,3,4]
    loop i: 8
        t[i] = x[i] * y[i];
}

//#[Swag.PrintBc]
#[Swag.NoInline]
#test
{
    var x: Vec4 = [1,2,3,4,1,2,3,4]
    var t: Vec4
    toto(x, t)
    //toto(x)
}