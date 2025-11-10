func print(f: f32)
    verify
    {
        if f != 3.14 do
            @compilererror("'f' should be 3.14", #curlocation)
        return true
    }
{
}

#test
{
    print(3.14)
}
