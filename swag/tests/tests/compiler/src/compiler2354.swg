#global testerror

//#[swag.mixin]
func mkVar(name: string, value: s32)
{
    #ast "titi := 0"
}

mkVar("titi", 555)

#placeholder titi
