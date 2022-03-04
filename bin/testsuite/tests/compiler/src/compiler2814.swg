#[Swag.Mixin]
func declare(value: s32)
{
    const @alias0: s32 = value
}

declare(|myVar| 5)
declare(|myVar1| 10)

#placeholder myVar
#assert myVar == 5
#placeholder myVar1
#assert myVar1 == 10