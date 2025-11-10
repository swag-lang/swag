#[Swag.Inline]
func titi()->s32 throw
{
    return 0
}

func toto()
{
    discard catch titi()
}
