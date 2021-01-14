func print(f: f32)
    #selectif
    {
        if f != 3.14 @error("'f' should be 3.14", #location)
        return true
    }
{

}

#test
{
    print(3.14)
}
