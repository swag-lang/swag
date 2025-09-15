#global testerror "Err0090"

#[Swag.NoCopy]
struct AA
{
}

struct BB
{
    x: AA
}

#test
{
    let x: BB
    let y = x
}