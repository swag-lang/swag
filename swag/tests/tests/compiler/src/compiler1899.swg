
#global testerror

public struct(T) TOTOcompiler1899
{
    #ast
    {
        @error(#location, "this is a test man !")
        return ""
    }
}

#test
{
    var x: TOTOcompiler1899's32()
}