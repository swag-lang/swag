#public
// zlib general purpose compression library
// version 1.2.11, January 15th, 2017
// Copyright (C) 1995-2017 Jean-loup Gailly and Mark Adler
// This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// This notice may not be removed or altered from any source distribution.
// Jean-loup Gailly       Mark Adler
// The data format used by the zlib library is described by RFCs (Request for Comments) 1950 to 1952 in the files rfc1950 (zlib format), rfc1951 (deflate format) and rfc1952 (gzip format).

// Error codes
const Z_OK            = 0
const Z_STREAM_END    = 1
const Z_NEED_DICT     = 2
const Z_ERRNO         = -1
const Z_STREAM_ERROR  = -2
const Z_DATA_ERROR    = -3
const Z_MEM_ERROR     = -4
const Z_BUF_ERROR     = -5
const Z_VERSION_ERROR = -6

// Compression levels
const Z_NO_COMPRESSION       = 0
const Z_BEST_SPEED           = 1
const Z_BEST_COMPRESSION     = 9
const Z_DEFAULT_COMPRESSION  = -1

#[swag.foreign("zlib")]
{
    func compress(dest: *u8, destLen: *u32, source: const *u8, sourceLen: u32)->s32;
    func compress2(dest: *u8, destLen: *u32, source: const *u8, sourceLen: u32, level: s32)->s32;
    func uncompress(dest: *u8, destLen: *u32, source: const *u8, sourceLen: u32)->s32;
    func compressBound(sourceLen: u32)->u32;
}