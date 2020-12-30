func titi()->{u32, u32} => @{0, 0}

#test
{
    {
        (idx, idx1) := titi()
    }

    (idx, idx1) := titi()
}