#global testerror "Err0024"

struct B5453
{
    using l: A5453
}

struct A5453
{
    parent: s32
}

impl B5453
{
    mtd toto(parent: bool)
    {
        if parent:
            @print("Q")
    }
}
