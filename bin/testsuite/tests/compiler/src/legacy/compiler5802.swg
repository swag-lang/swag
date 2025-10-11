namespace Win32
{
    func toto() {}
}

enum AA { R }

#test
{
    with AA
    {
        let a = R
    }

    with Win32
    {
        toto()
    }
}
