#global skip
#global testwarnings

interface I
{
    mtd toto()
}

struct A
{
}

impl I for A
{
    #[Swag.Warn("wrn0004", Swag.WarnLevel.Enable)]
    mtd titi(x: s32)
    {
    }

    #[Swag.Warn("wrn0004", Swag.WarnLevel.Enable)]
    mtd toto()
    {
    }
}