#global testerror

public struct(T) TOTOcompiler1899
{
    #ast
    {
        @compilererror("this is a test man !", #location)
        return ""
    }
}

#test
{
    var x: TOTOcompiler1899's32{}
}