#global testerror "Err0096"
struct(T) A {}
struct Str {}

impl Str
{
    mtd opAffect(x: s32) {}
}

impl A
{
    #[Swag.Overload]
    mtd add(x: const [..] T) {}
    #[Swag.Overload]
    mtd add(x: T) {}
}

#test
{
    var a: A'Str
    a.add(0)
}
