#[Swag.Strict]
alias U = u32

#[Swag.Strict]
alias V = u32

func toto(u: V) {}

func toto1()
{
    var x: U
    toto(cast() x)
}
