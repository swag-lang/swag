import { defineConfig } from 'vite';
import laravel from 'laravel-vite-plugin';
//require('dotenv').config();

export default defineConfig({
    plugins: [
        laravel({
            input: [
                'resources/sass/app.scss',
                'resources/js/app.js'
            ],
            refresh: true,
        }),
    ],
    server: {
        //host: process.env.APP_URL.replace('http://', '').replace('https://', ''),
    },
    css: {
        postCss: {
            plugins: {
                tailwindcss: {},
                autoprefixer: {},
              },
        },
    },
});
