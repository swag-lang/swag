func titi()->{a,b: u32} => {0, 0}

#test
{
    {
        (idx, idx1) := titi()
    }

    (idx, idx1) := titi()
}