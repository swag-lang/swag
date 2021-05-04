#global if #os == "windows"
using core, win32, ogl, gdi32, time

var hDC:            HDC
var vertexbuffer:   GLuint
var programID:      GLuint

const g_vertex_buffer_data: [] GLfloat = @[
    -1.0, -1.0, 0.0,
     1.0, -1.0, 0.0,
     0.0,  1.0, 0.0,
]

const g_vertex_shader = @"
    #version 330 core
    layout(location = 0) in vec3 vertexPosition_modelspace;
    void main()
    {
        gl_Position.xyz = vertexPosition_modelspace;
        gl_Position.w = 1.0;
    }
"@

const g_pixel_shader = @"
    #version 330 core
    out vec3 color;
    void main()
    {
        color = vec3(1,0,0);
    }
"@

func display()
{
    glClearColor(0, 0, 0, 0)
    glClear(GL_COLOR_BUFFER_BIT)

    glUseProgram(programID);
    glEnableVertexAttribArray(0)
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, null)
    glDrawArrays(GL_TRIANGLES, 0, 3) // Starting from vertex 0; 3 vertices total -> 1 triangle
    glDisableVertexAttribArray(0)

    glFlush()
    assume SwapBuffers(hDC)
}

#[swag.callback]
func winProc(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT
{
    switch msg
    {
    case WM_PAINT:
        display()
        var ps: PAINTSTRUCT
        BeginPaint(hWnd, &ps)
        EndPaint(hWnd, &ps)
        return 0

    case WM_SIZE:
	    glViewport(0, 0, LOWORD(lParam), HIWORD(lParam))
	    assume PostMessageA(hWnd, WM_PAINT, 0, 0)
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
    wc.lpfnWndProc   = acast @mkcallback(&winProc)
    wc.hInstance     = GetModuleHandleA(null)
    wc.lpszClassName = @dataof("CLASS_NAME")
    assume RegisterClassA(wc)

    hWnd := CreateWindowExA(
        0,
        @dataof("CLASS_NAME"),
        @dataof("Example"),
        WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        512,
        512,
        null,
        null,
        wc.hInstance,
        null)

    hDC = GetDC(hWnd)

    var pfd: PIXELFORMATDESCRIPTOR
    pfd.nSize        = @sizeof(pfd)
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL
    pfd.iPixelType   = PFD_TYPE_RGBA
    pfd.cColorBits   = 32

    pf := assume ChoosePixelFormat(hDC, &pfd)
    assume SetPixelFormat(hDC, pf, &pfd)
    assume DescribePixelFormat(hDC, pf, cast(UINT) @sizeof(PIXELFORMATDESCRIPTOR), &pfd)

    ReleaseDC(hDC, hWnd)
    return hWnd
}

func messageLoop()
{
    var msg: MSG
    while true
    {
        while PeekMessageA(&msg, null, 0, 0, PM_REMOVE) != 0
        {
            if msg.message == WM_QUIT
                return
            TranslateMessage(&msg)
            DispatchMessageA(&msg)
        }

        display()
    }
}

func createShader()
{
	var result: GLint
	var infoLogLength: s32
    var infoLog: String

    vertexShaderID := glCreateShader(GL_VERTEX_SHADER)
    vertexSourcePointer := @dataof(g_vertex_shader)
    glShaderSource(vertexShaderID, 1, &vertexSourcePointer, null)
	glCompileShader(vertexShaderID)
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result)
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength)
    infoLog.reserve(cast(uint) infoLogLength + 1)
    glGetShaderInfoLog(vertexShaderID, infoLogLength, null, @dataof(infoLog))

	fragmentShaderID := glCreateShader(GL_FRAGMENT_SHADER)
    pixelSourcePointer := @dataof(g_pixel_shader)
    glShaderSource(fragmentShaderID, 1, &pixelSourcePointer, null)
	glCompileShader(fragmentShaderID)
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result)
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength)
    infoLog.reserve(cast(uint) infoLogLength + 1)
    glGetShaderInfoLog(fragmentShaderID, infoLogLength, null, @dataof(infoLog))

    programID = glCreateProgram()
	glAttachShader(programID, vertexShaderID)
	glAttachShader(programID, fragmentShaderID)
	glLinkProgram(programID)
    glGetProgramiv(programID, GL_LINK_STATUS, &result)
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength)
    glGetProgramInfoLog(programID, infoLogLength, null, @dataof(infoLog))

    glDetachShader(programID, vertexShaderID)
	glDetachShader(programID, fragmentShaderID)
	glDeleteShader(vertexShaderID)
	glDeleteShader(fragmentShaderID)
}

#main
{
    hWnd := createWindow()

    hDC = GetDC(hWnd)
    hRC := try wglCreateContext(hDC)
    try wglMakeCurrent(hDC, hRC)
    glInitExtensions()

    glGenBuffers(1, &vertexbuffer)
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, @sizeof(g_vertex_buffer_data), @dataof(g_vertex_buffer_data), GL_STATIC_DRAW);
    createShader()

    display()
    ShowWindow(hWnd, SW_SHOW)
    try UpdateWindow(hWnd)

    messageLoop()

    try wglMakeCurrent(null, null)
    try wglDeleteContext(hRC)
    ReleaseDC(hWnd, hDC)
}