#global testerror "Err0053"
struct(T) A {}

impl A
{
    mtd add(x: T) {}
    //mtd add(x: const [..] T) {}
}

#test
{
    let a: A's32
    a.add(0)
}