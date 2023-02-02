func print(f: f32)
    #selectifx
    {
        if f != 3.14 @compilererror("'f' should be 3.14", #location)
        return true
    }
{
}

#test
{
    print(3.14)
}
