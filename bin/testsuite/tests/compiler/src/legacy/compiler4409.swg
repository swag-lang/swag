#global testerror "Err0561"
struct(T) A {}
struct Str {}

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
