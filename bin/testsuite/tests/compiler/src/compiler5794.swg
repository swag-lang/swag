//using test_foreign

interface I
{
    mtd call() {}
}

struct AA {}

impl I for AA
{
}

func toto()
{
    let x:  AA
    let itf = cast(I) x
    itf.call()
}
