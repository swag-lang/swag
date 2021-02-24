func print(f: f32)
    #checkif
    {
        if f != 3.14 @errormsg("'f' should be 3.14", #location)
        return true
    }
{
}

#test
{
    print(3.14)
}
