#global testerror

(x, y) := toto()

#[Swag.constexpr]
func toto() -> {x: s32, y: s32}
{
    return @{10, 20}
}