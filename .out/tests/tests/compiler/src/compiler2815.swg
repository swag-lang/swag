#[swag.mixin]
func declare(name: string, value: s32)
{
    #ast "const " ~name~ "2 = " ~value
}

declare("myVar", 5)
#placeholder myVar2
#assert myVar2 == 5
