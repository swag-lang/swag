#global testerror

public struct(T) TOTOcompiler1899
{
    #ast
    {
        @error("this is a test man !", #location)
        return ""
    }
}

#test
{
    var x: TOTOcompiler1899's32{}
}