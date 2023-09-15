interface I
{
    mtd toto()
}

struct A
{
}

impl I for A
{
    #[Swag.Warn("wrn0002", Swag.WarnLevel.Enable)]
    mtd impl toto()
    {
    }
}