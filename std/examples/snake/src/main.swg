#import "std"
using win32, std.keyboard

var nScreenWidth  = 120'u32
var nScreenHeight = 30'u32

struct SnakeSegment
{
	x, y: s32
};

func main()
{
    var screen: *u8 = @alloc(nScreenWidth*nScreenHeight)
    loop i: nScreenWidth*nScreenHeight
        screen[i] = " "'u8
    hConsole := CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, null, CONSOLE_TEXTMODE_BUFFER, null)
	SetConsoleActiveScreenBuffer(hConsole)
	var dwBytesWritten: DWORD = 0

    nSnakeDirection := 3
    nFoodX := 30
    nFoodY := 15
    nScore := 0
    var rand: random.Random
    var keyb: keyboard.Keyboard

    var snake: Array'SnakeSegment = @[ @{60,15}, @{61,15}, @{62,15}, @{63,15}, @{64,15}, @{65,15}, @{66,15}, @{67,15}, @{68,15}, @{69,15} ]

    bDead := false
    while !bDead
    {
        thread.sleep((nSnakeDirection % 2 == 1) ? 120 : 200)

        // Input ========================
        using keyboard.Key
        keyb.update()
        if keyb.isKeyPressed(Escape)
            break

        if keyb.isKeyJustPressed(Up)
            nSnakeDirection = 0
        if keyb.isKeyJustPressed(Right)
            nSnakeDirection = 1
        if keyb.isKeyJustPressed(Down)
            nSnakeDirection = 2
        if keyb.isKeyJustPressed(Left)
            nSnakeDirection = 3

        // ==== Logic

        // Update Snake Position, place a new head at the front of the list in
        // the right direction
        switch nSnakeDirection
        {
        case 0: // UP
            snake.insertAt(0, @{ snake.front().x, snake.front().y - 1 })
        case 1: // RIGHT
            snake.insertAt(0, @{ snake.front().x + 1, snake.front().y })
        case 2: // DOWN
            snake.insertAt(0, @{ snake.front().x, snake.front().y + 1 })
        case 3: // LEFT
            snake.insertAt(0, @{ snake.front().x - 1, snake.front().y })
        }

        // Collision Detect Snake V Food
        if snake.front().x == nFoodX && snake.front().y == nFoodY
        {
            nScore += 1
            while screen[nFoodY * nScreenWidth + nFoodX] != " "'u8
            {
                nFoodX = acast (rand.nextU32() % nScreenWidth)
                nFoodY = acast ((rand.nextU32() % (nScreenHeight-3))+3)
            }

            for i := 0; i < 5; i += 1
                snake.add(@{ snake.back().x, snake.back().y })
        }

        // Collision Detect Snake V World
        if snake.front().x < 0 || snake.front().x >= nScreenWidth
            bDead = true
        if snake.front().y < 3 || snake.front().y >= nScreenHeight
            bDead = true

        // Collision Detect Snake V Snake
        visit *i: snake
            if @index && i.x == snake.front().x && i.y == snake.front().y
                bDead = true

    	// Chop off Snakes tail :-/
		snake.removeBack()

        // ==== Presentation

        // Clear Screen
        loop i: nScreenWidth*nScreenHeight
            screen[i] = " "'u8

        // Draw Stats & Border
        for i := 0; i < nScreenWidth; i += 1
        {
            screen[i] = "="'u8
            screen[2 * nScreenWidth + i] = "="'u8
        }

        // Draw Snake Body
        visit *s: snake
            screen[s.y * nScreenWidth + s.x] = bDead ? "+"'u8 : "O"'u8

        // Draw Snake Head
        screen[snake.front().y * nScreenWidth + snake.front().x] = bDead ? "X"'u8 : "@"'u8

        // Draw Food
        screen[nFoodY * nScreenWidth + nFoodX] = "%"'u8

		// Display Frame
        WriteConsoleOutputCharacterA(hConsole, screen, nScreenWidth * nScreenHeight, 0, &dwBytesWritten);
    }

    CloseHandle(hConsole)
}

struct sSnakeSegment
{
	x, y: s32
}

//#run main()

#main
{
    main()
}