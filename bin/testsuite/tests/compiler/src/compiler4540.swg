#global testerror "Err0481"

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