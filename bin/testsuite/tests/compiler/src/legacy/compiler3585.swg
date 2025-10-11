struct(T) A {}
impl A
{
    mtd opAffect(v: T) {}
}

#test
{
    var x: A'func||(s32)
    var y: A'func||(f32)

    x = func(t) {}
    y = func(t) {}
}
