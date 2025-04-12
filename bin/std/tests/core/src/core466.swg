using Core

#[Swag.PrintBc]
#test
{
    var a: Array'String
    a.add("toto")
    a[0] = String.from("titi")
}
