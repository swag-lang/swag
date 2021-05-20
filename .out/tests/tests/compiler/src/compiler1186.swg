struct TOTO
{
    x: bool
    #[Swag.offset("x")]
    y: s32
}

#assert @sizeof(TOTO) == 4