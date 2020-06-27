interface ITOTO
{
    ptr: func(self)
}

var val: const ITOTO

func fc(x: ITOTO)
{
    val = x
}