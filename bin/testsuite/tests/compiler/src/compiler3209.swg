#global testerror "Err0750"
func ReleaseDC()->s32 => 0

func createContext(hWnd: *void)
{
    defer ReleaseDC()
}
