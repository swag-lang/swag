<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="icon" type="image/x-icon" href="favicon.ico">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>{{ env('APP_ENV') == 'local' ? '[DEV] ' : '' }}Swag</title>
    @vite('resources/sass/app.scss')
    <script defer src="https://cdn.jsdelivr.net/npm/alpinejs@3.12.1/dist/cdn.min.js"></script>
</head>
<body>

    @include('common.header')

    @yield('layout')

</body>
</html>