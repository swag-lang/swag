interface ITOTO
{
    ptr: func(self)
}

var val: ITOTO

func fc(x: ITOTO)
{
    val = x
}