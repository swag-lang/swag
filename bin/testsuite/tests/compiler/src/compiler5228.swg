struct A
{
    a:     I
    b:     J
    c:     K
}

interface I
{
    mtd f();
}
interface J
{
    mtd f();
}
interface K
{
    mtd f();
}

#assert(#typeof(A).name == "A")
