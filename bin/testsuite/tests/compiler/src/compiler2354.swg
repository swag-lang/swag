#global testerror "Err0686"

//#[Swag.Mixin]
func mkVar(name: string, value: s32)
{
    #ast "var titi = 0"
}

mkVar("titi", 555)

#placeholder titi
