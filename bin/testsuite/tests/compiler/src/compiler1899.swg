#global testerror "this is a test man !"

public struct(T) TOTOcompiler1899
{
    #ast
    {
        @compilererror("this is a test man !", #curlocation)
        return ""
    }
}

#test
{
    var x: TOTOcompiler1899's32{}
}