#global testerror "Err0113"
struct(T) A {}
struct Str {}

impl A
{
    mtd add(x: const [..] T) {}
    mtd add(x: T) {}
}

#test
{
    var a: A'Str
    a.add(0)
}