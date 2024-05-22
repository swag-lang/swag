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
    #[Swag.Warn("Wrn0006", Swag.WarnLevel.Enable)]
    mtd titi(x: s32)
    {
    }

    #[Swag.Warn("Wrn0006", Swag.WarnLevel.Enable)]
    mtd impl toto()
    {
    }
}
