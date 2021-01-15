func print(f: ...)
    #selectif
    {
        @print(cast(s64) @countof(f))
        return true
    }
{
}

#[swag.printbc, swag.optim(false)]
#test
{
    print(1, 2, 3)
}
