func titi()->{ a, b: u32 } => {0, 0}

#test
{
    {
        var (idx, idx1) = titi()
    }

    var (idx, idx1) = titi()
}
