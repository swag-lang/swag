struct AA
{
    x: s32
    y: s32
}

#assert @countof(@typeof(AA).fields) == 2