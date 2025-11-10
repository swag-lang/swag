alias Vec4 = [8] f32

//#[Swag.PrintBc]
#[Swag.NoInline]
func toto(x: Vec4, t: Vec4)
{
    let y: Vec4 = [1, 2, 3, 4, 1, 2, 3, 4]
    for i in 8 do
        t[i] = x[i] * y[i]
}

//#[Swag.PrintBc]
#[Swag.NoInline]
#test
{
    let x: Vec4 = [1, 2, 3, 4, 1, 2, 3, 4]
    var t: Vec4
    toto(x, t)
    //toto(x)
}
