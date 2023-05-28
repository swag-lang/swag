<header class="sticky top-0 z-50">
    <div class="lg:m-auto w-full lg:w-[75rem] lg:flex justify-between items-center p-2" x-data="{ open_menu: false }">
        <div class="flex justify-between items-center">
            <a href="index.php"><img src="imgs/swag-logo.jpg" alt="SWAG" class="w-24 lg:w-50" /></a>
            <div class="lg:hidden">
                <img src="imgs/burger.png" alt="" class="w-10 cursor-pointer" @click="open_menu = ! open_menu" />
            </div>
        </div>
        <ul class="text-white text-md mt-3 lg:mt-0" :class="open_menu ? '' : 'hidden lg:flex'">
            <li class="lg:mr-5 mb-1 lg:mb-0 ml-2 lg:ml-0"><a href="index.php" class="<?=$_SERVER['REQUEST_URI'] == '/index.php' ? 'border-b' : ''?> hover:border-b">Home</a></li>
            <li class="lg:mr-5 mb-1 lg:mb-0 ml-2 lg:ml-0"><a href="language.php" class="<?=$_SERVER['REQUEST_URI'] == '/language.php' ? 'border-b' : ''?> hover:border-b">Overview</a></li>
            <li class="lg:mr-5 mb-1 lg:mb-0 ml-2 lg:ml-0"><a href="std/swag.runtime.html" target="_blank" class="hover:border-b">Runtime</a></li>
            <li class="lg:mr-5 mb-1 lg:mb-0 ml-2 lg:ml-0"><a href="libraries.php" class="<?=$_SERVER['REQUEST_URI'] == '/libraries.php' ? 'border-b' : ''?> hover:border-b">Std</a></li>
            <li class="mb-1 lg:mb-0 ml-2 lg:ml-0"><a href="https://github.com/swag-lang/swag" target="_blank" class="hover:border-b">Github</a></li>
        </ul>
    </div>
</header>