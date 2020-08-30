struct(T) C1914
{
    x, y: T
}

impl C1914
{
    func toto(using self)
    {
        x, y = 10
    }
}

//alias TC1914 = C1914's32
//alias TC19142 = C1914'f32

#test
{
    var x : C1914's32
    var y : C1914'f32
    @print(@typeof(x).name)
    @print(@typeof(y).name)
}