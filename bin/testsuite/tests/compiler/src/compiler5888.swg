#[Swag.Mixin]
func newCmdId(val: string)
{
    #ast "const " ++ val ++ " = \"" ++ val ++ "\""
}

struct A {}

impl A
{
    #ast "const TOTO = 0"
}

#test
{
    let a = A.TOTO
}
