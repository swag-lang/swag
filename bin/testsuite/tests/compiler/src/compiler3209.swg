#global testerror "Err0748"
func ReleaseDC()->s32 => 0

func createContext(hWnd: *void)
{
    defer ReleaseDC()
}
