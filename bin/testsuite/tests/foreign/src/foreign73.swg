#[Swag.Mixin]
func toto()
{
    #ast "const A = 0"
}

public struct B73 {}
public impl B73
{
    toto()
}