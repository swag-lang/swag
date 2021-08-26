#[Swag.Strict]
{
    alias AA = *void
    alias BB = *void
}

func test(x: AA)
{
}

func test(x: BB)
{
}
/*
#test
{
    var vvv: AA
    test(vvv)
}*/