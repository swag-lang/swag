#[Swag.Strict]
typealias U = u32

#[Swag.Strict]
typealias V = u32

func toto(u: V)
{
}

func toto1()
{
    var x: U
    toto(acast x)
}