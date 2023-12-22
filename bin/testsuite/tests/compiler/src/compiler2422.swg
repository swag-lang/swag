func print(f: f32)
    #validifx
    {
        if f != 3.14  do @compilererror("'f' should be 3.14", #location)
        return true
    }
{
}

#test
{
    print(3.14)
}
