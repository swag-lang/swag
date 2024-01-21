#global testerror "Err0536"

interface I
{
    mtd toto()
}

struct S {}

impl I for S
{
    mtd toto() {}
}