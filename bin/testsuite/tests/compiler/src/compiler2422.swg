func print(f: f32)
    where<call>
    {
        if f != 3.14 : @compilererror("'f' should be 3.14", #curlocation)
        return true
    }
{
}

#test
{
    print(3.14)
}
