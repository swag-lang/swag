#global testerror "Err0071"

#[Swag.EnumFlags]
enum Toto
{
    A
    B
    C
}

#test
{
    var x: Toto = A
    if x.C
    {
        @print("A")
    }
}
