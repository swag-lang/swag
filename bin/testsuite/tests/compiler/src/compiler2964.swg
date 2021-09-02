struct AA
{
    x: s32
    #[Swag.Offset("x")]
    y, z: s32
}

#assert @offsetof(AA.x) == 0
#assert @offsetof(AA.y) == 0
#assert @offsetof(AA.z) == 0
#assert @sizeof(AA) == 4