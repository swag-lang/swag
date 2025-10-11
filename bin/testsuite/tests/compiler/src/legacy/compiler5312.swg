#[Swag.Sanity(false)]
func toto(x: string)->u64
{
    var tt = 0'u64
    for x do
        tt += 1

    let y = tt - 2
    return y
}

#test
{
    discard toto("aaa")
}
