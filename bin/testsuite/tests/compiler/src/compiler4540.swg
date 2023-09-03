#global testerror "Err0488"

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