struct TOTO
{
    x: bool
    #[swag.offset("x")]
    y: s32
}

#assert @sizeof(TOTO) == 4