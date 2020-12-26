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

//public #bake C1914s32 = C1914's32
//public #bake C1914f32 = C1914'f32

alias TC1914 = C1914's32
alias TC19142 = C1914'f32

#test
{
    var x : TC1914
    var y : TC19142
    @assert(@typeof(x).name == "__compiler1914.C1914'(s32)")
    @assert(@typeof(y).name == "__compiler1914.C1914'(f32)")
}