#[Swag.Compiler]
const X = @[10, 10, 20, 40]

#[Swag.Compiler]
func toto()
{
    func titi()
    {
        @print(X[0])
    }
}