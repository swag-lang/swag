#global testerror "Err0535"

interface I
{
    mtd toto()
}

struct S {}

impl I for S
{
    mtd toto() {}
}