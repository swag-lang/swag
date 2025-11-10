enum X: const [4] u8
{
    A = [1, 2, 3, 4]
    B = [10, 20, 30, 40]
}

#test
{
    let ptr = #typeof(X)
    var cpt = 0
    foreach p in ptr.values
    {
        let ptr1 = cast(const *u8) p.value
        cpt += dref ptr1
    }

    @assert(cpt == 11)
}
