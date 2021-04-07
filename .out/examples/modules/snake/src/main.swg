/*
OneLoneCoder.com - Command Line Snake
"Give me a break, I'm on holiday..." - @Javidx9
License
~~~~~~~
Copyright (C) 2018  Javidx9
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; See license for details.
Original works located at:
https://www.github.com/onelonecoder
https://www.onelonecoder.com
https://www.youtube.com/javidx9
GNU GPLv3
https://github.com/OneLoneCoder/videos/blob/master/LICENSE
From Javidx9 :)
~~~~~~~~~~~~~~~
Hello! Ultimately I don't care what you use this for. It's intended to be
educational, and perhaps to the oddly minded - a little bit of fun.
Please hack this, change it and use it in any way you see fit. You acknowledge
that I am not responsible for anything bad that happens as a result of
your actions. However this code is protected by GNU GPLv3, see the license in the
github repo. This means you must attribute me if you use it. You can view this
license here: https://github.com/OneLoneCoder/videos/blob/master/LICENSE
Cheers!
Background
~~~~~~~~~~
Classic Snake! Controls are Arrow keys Left & Right, eat food, grow larger, avoid self!
Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com
Video:
~~~~~~
https://youtu.be/e8lYLYlrGLg
Last Updated: 24/06/2017
*/
#global if #os == "windows"
using win32, core.keyboard

var nScreenWidth  = 120'u32
var nScreenHeight = 30'u32

struct SnakeSegment
{
	x, y: s32
};

func main()
{
    var screen: *u8 = @alloc(cast(uint) nScreenWidth*nScreenHeight)
    loop i: nScreenWidth*nScreenHeight
        screen[i] = " "'u8
    hConsole := CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, null, CONSOLE_TEXTMODE_BUFFER, null)
	SetConsoleActiveScreenBuffer(hConsole)
	var dwBytesWritten: DWORD = 0

    nSnakeDirection := 3
    nFoodX := 30
    nFoodY := 15
    nScore := 0
    var rand: random.Rng
    var keyb: keyboard.Keyboard

    var snakeArr: Array'SnakeSegment = @[ @{60,15}, @{61,15}, @{62,15}, @{63,15}, @{64,15}, @{65,15}, @{66,15}, @{67,15}, @{68,15}, @{69,15} ]

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
            snakeArr.insertAt(0, @{ snakeArr.front().x, snakeArr.front().y - 1 })
        case 1: // RIGHT
            snakeArr.insertAt(0, @{ snakeArr.front().x + 1, snakeArr.front().y })
        case 2: // DOWN
            snakeArr.insertAt(0, @{ snakeArr.front().x, snakeArr.front().y + 1 })
        case 3: // LEFT
            snakeArr.insertAt(0, @{ snakeArr.front().x - 1, snakeArr.front().y })
        }

        // Collision Detect Snake V Food
        if snakeArr.front().x == nFoodX && snakeArr.front().y == nFoodY
        {
            nScore += 1
            while screen[nFoodY * nScreenWidth + nFoodX] != " "'u8
            {
                nFoodX = autocast (rand.nextU32() % nScreenWidth)
                nFoodY = autocast ((rand.nextU32() % (nScreenHeight-3))+3)
            }

            for i := 0; i < 5; i += 1
                snakeArr.add(@{snakeArr.back().x, snakeArr.back().y})
        }

        // Collision Detect Snake V World
        if snakeArr.front().x < 0 || snakeArr.front().x >= nScreenWidth
            bDead = true
        if snakeArr.front().y < 3 || snakeArr.front().y >= nScreenHeight
            bDead = true

        // Collision Detect Snake V Snake
        visit *i: snakeArr
            if @index && i.x == snakeArr.front().x && i.y == snakeArr.front().y
                bDead = true

    	// Chop off Snakes tail :-/
		snakeArr.removeBack()

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
        visit *s: snakeArr
            screen[s.y * nScreenWidth + s.x] = bDead ? "+"'u8 : "O"'u8

        // Draw Snake Head
        screen[snakeArr.front().y * nScreenWidth + snakeArr.front().x] = bDead ? "X"'u8 : "@"'u8

        // Draw Food
        screen[nFoodY * nScreenWidth + nFoodX] = "%"'u8

		// Display Frame
        wlen := WriteConsoleOutputCharacterA(hConsole, screen, nScreenWidth * nScreenHeight, 0, &dwBytesWritten);
    }

    CloseHandle(hConsole)
}

//#run main()

#main
{
    main()
}