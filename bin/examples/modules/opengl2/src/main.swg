#global #if #os == Swag.TargetOs.Windows
using Core, Win32, Ogl, Gdi32, Core.Time

var g_HDC:          HDC
var g_VertexBuffer: GLuint
var g_ProgramID:    GLuint

const VertexBufferData: [?] GLfloat = [
    -1.0, -1.0, 0.0,
    1.0, -1.0, 0.0,
    0.0, 1.0, 0.0]

const VertexShader = """
    #version 330 core
    layout(location = 0) in vec3 vertexPosition_modelspace;
    out vec3 vcolor;
    void main()
    {
        gl_Position.xyz = vertexPosition_modelspace;
        gl_Position.w = 1.0;
        vcolor = vec3(1,0,1);
    }
"""

const PixelShader = """
    #version 330 core
    in vec3 vcolor;
    out vec3 color;
    void main()
    {
        color = vcolor;//vec3(1,0,0);
    }
"""

func display()
{
    glClearColor(0, 0, 0, 0)
    glClear(GL_COLOR_BUFFER_BIT)

    glUseProgram(g_ProgramID)
    glEnableVertexAttribArray(0)
    glBindBuffer(GL_ARRAY_BUFFER, g_VertexBuffer)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, null)
    glDrawArrays(GL_TRIANGLES, 0, 3)
    glDisableVertexAttribArray(0)

    glFlush()
    assume SwapBuffers(g_HDC)
}

func winProc(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT
{
    switch msg
    {
    case WM_PAINT:
        display()

    case WM_SIZE:
        glViewport(0, 0, LOWORD(lParam), HIWORD(lParam))
        display()
        return 0

    case WM_DESTROY:
        PostQuitMessage(0)
    }

    return DefWindowProcA(hWnd, msg, wParam, lParam)
}

func createWindow()->HWND
{
    var wc: WNDCLASSA
    wc.style         = CS_OWNDC
    wc.lpfnWndProc   = cast() @mkcallback(&winProc)
    wc.hInstance     = GetModuleHandleA(null)
    wc.lpszClassName = @dataof("CLASS_NAME")
    assume RegisterClassA(wc)

    let hWnd = CreateWindowExA(0, @dataof("CLASS_NAME"), @dataof("Example"), WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, 512, 512, null, null, wc.hInstance, null)

    g_HDC = GetDC(hWnd)

    var pfd: PIXELFORMATDESCRIPTOR
    pfd.nSize      = #sizeof(pfd)
    pfd.nVersion   = 1
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL
    pfd.iPixelType = PFD_TYPE_RGBA
    pfd.cColorBits = 32

    let pf = assume ChoosePixelFormat(g_HDC, &pfd)
    assume SetPixelFormat(g_HDC, pf, &pfd)
    assume DescribePixelFormat(g_HDC, pf, cast(UINT) #sizeof(PIXELFORMATDESCRIPTOR), &pfd)

    ReleaseDC(hWnd, g_HDC)
    return hWnd
}

func messageLoop()
{
    var msg: MSG
    var cpt  = 0
    while cpt < 100
    {
        while PeekMessageA(&msg, null, 0, 0, PM_REMOVE) != 0
        {
            if msg.message == WM_QUIT do
                return
            TranslateMessage(&msg)
            DispatchMessageA(&msg)
        }

        display()

        if Env.hasArg("swag.test") do
            cpt += 1
    }
}

func createShader()
{
    var result:        GLint
    var infoLogLength: s32
    var infoLog:       String

    let vertexShaderID      = glCreateShader(GL_VERTEX_SHADER)
    var vertexSourcePointer = @dataof(VertexShader)
    glShaderSource(vertexShaderID, 1, &vertexSourcePointer, null)
    glCompileShader(vertexShaderID)
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result)
    glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength)
    infoLog.reserve(cast(u64) infoLogLength + 1)
    glGetShaderInfoLog(vertexShaderID, infoLogLength, null, @dataof(infoLog))

    let fragmentShaderID   = glCreateShader(GL_FRAGMENT_SHADER)
    var pixelSourcePointer = @dataof(PixelShader)
    glShaderSource(fragmentShaderID, 1, &pixelSourcePointer, null)
    glCompileShader(fragmentShaderID)
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result)
    glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength)
    infoLog.reserve(cast(u64) infoLogLength + 1)
    glGetShaderInfoLog(fragmentShaderID, infoLogLength, null, @dataof(infoLog))

    g_ProgramID = glCreateProgram()
    glAttachShader(g_ProgramID, vertexShaderID)
    glAttachShader(g_ProgramID, fragmentShaderID)
    glLinkProgram(g_ProgramID)
    glGetProgramiv(g_ProgramID, GL_LINK_STATUS, &result)
    glGetProgramiv(g_ProgramID, GL_INFO_LOG_LENGTH, &infoLogLength)
    glGetProgramInfoLog(g_ProgramID, infoLogLength, null, @dataof(infoLog))

    glDetachShader(g_ProgramID, vertexShaderID)
    glDetachShader(g_ProgramID, fragmentShaderID)
    glDeleteShader(vertexShaderID)
    glDeleteShader(fragmentShaderID)
}

#main
{
    let hWnd = createWindow()

    g_HDC = GetDC(hWnd)
    let hRC = try wglCreateContext(g_HDC)
    try wglMakeCurrent(g_HDC, hRC)
    glInitExtensions()

    glGenBuffers(1, &g_VertexBuffer)
    glBindBuffer(GL_ARRAY_BUFFER, g_VertexBuffer)
    glBufferData(GL_ARRAY_BUFFER, #sizeof(VertexBufferData), @dataof(VertexBufferData), GL_STATIC_DRAW)
    createShader()

    display()
    ShowWindow(hWnd, SW_SHOW)
    try UpdateWindow(hWnd)

    messageLoop()

    try wglMakeCurrent(null, null)
    try wglDeleteContext(hRC)
    ReleaseDC(hWnd, g_HDC)
}
