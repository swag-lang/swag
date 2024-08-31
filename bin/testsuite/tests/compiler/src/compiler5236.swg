#global testerror "Err0546"
struct(T) A {}

impl A
{
    #[Swag.Overload]
    mtd add(x: T) {}
    #[Swag.Overload]
    mtd add(x: const [..] T) {}
}

#test
{
    let a: A's32
    a.add(0)
}