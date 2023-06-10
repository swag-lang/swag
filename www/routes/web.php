<?php

use Illuminate\Support\Facades\Route;

use App\Http\Controllers\AppController;

/*
|--------------------------------------------------------------------------
| Web Routes
|--------------------------------------------------------------------------
|
| Here is where you can register web routes for your application. These
| routes are loaded by the RouteServiceProvider and all of them will
| be assigned to the "web" middleware group. Make something great!
|
*/

Route::controller(AppController::class)->group(function() {
    Route::get('/', 'home')->name('home');
    Route::get('/std', 'std')->name('std');
    Route::get('/overview', 'overview')->name('overview');
    Route::get('/getting-started', 'getting_started')->name('getting-started');
    Route::get('/swag-as-script', 'swag_as_script')->name('swag-as-script');
});