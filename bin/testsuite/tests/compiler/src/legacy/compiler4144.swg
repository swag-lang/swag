#global testwarning "Wrn0006"

interface I
{
    mtd toto();
}

struct A
{
}

impl I for A
{
    #[Swag.Warning("Wrn0006", Swag.WarningLevel.Enable)]
    mtd titi(x: s32)
    {
    }

    #[Swag.Warning("Wrn0006", Swag.WarningLevel.Enable)]
    mtd impl toto()
    {
    }
}
