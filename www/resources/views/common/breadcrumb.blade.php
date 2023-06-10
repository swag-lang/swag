<nav class="mb-5 pb-1 border-b">
    <ol class="flex items-center space-x-1 md:space-x-2">
        <li>
            <a href="{{ route('home') }}">Home</a>
        </li>
        <li>
            <img src="/imgs/chevron.png" alt="" class="w-4" />
        </li>
        <li>
            {{ $text ?? '' }}
        </li>
    </ol>
</nav>