struct(T) A {}
impl A
{
    mtd opAffect(v: T) {}
}

#test
{
    var x: A'closure(s32)
    var y: A'closure(f32)

    x = func(t) {}
    y = func(t) {}
}