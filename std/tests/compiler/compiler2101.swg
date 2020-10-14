typeset AA
{
    x: {s32}
    y: {s32, f32}
}

#[swag.printbc]
#test
{
    var v: AA
    v = AA.x{666}
}