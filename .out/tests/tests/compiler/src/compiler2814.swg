#[swag.mixin]
func declare(name: alias, value: s32)
{
    const name: s32 = value
}

declare(#code myVar, 5)
declare(#code myVar1, 10)

#placeholder myVar
#assert myVar == 5
#placeholder myVar1
#assert myVar1 == 10