#global testwarning "Wrn0009"
#global #[Swag.Warning("Wrn0009", Swag.WarningLevel.Enable)]

const g: bool = true
#if g == false:
    const g1: bool = true
#else:
    #if g == true:
        const g1: bool = true
