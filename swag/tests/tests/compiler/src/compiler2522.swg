#global testerror

(x, y) := toto()

#[swag.constexpr]
func toto() -> {x: s32, y: s32}
{
    return @{10, 20}
}