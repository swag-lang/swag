func toto(x: string)->u64
{
    var tt = 0'u64
    for x:
        tt += 1

    #[Swag.Safety("sanity", false)]
    let y = tt-2
    return y
}

#test
{
    discard toto("aaa")
}