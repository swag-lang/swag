//#[Swag.PrintBc]
#test
{
    let x  = "x"
    let y  = "y"
    var xp = @dataof(x)
    let yp = @dataof(y)

    xp = #move yp
}
