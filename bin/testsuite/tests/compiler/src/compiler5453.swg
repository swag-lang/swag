#global testerror "Err0017"

struct A5453
{
    parent: s32
}

struct B5453
{
    using l: A5453
}

impl B5453
{
    mtd toto(parent: bool)
    {
        if parent do
            @print("Q")
    }
}