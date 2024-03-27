struct TOTO
{
    x: bool
    #[Swag.Offset("x")]
    y: s32
}

#assert @sizeof(TOTO) == 4