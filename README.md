This is plugin for GROK HTJ2K version https://github.com/GrokImageCompression/grok/commit/9cd001d3bb41e4a24e4fbaea95855110fbf3068a.

The latest GROK is in language C⁺⁺20. So we can theoretically disable `SPDLOG` function for C⁺⁺11/14/17 by defining SPDLOG_USE_STD_FORMAT.

The problem is that PNMFormat addon doesn't have `std::formated<PNM_COLOUR_SPACE>` for C⁺⁺20.

How to use GCC under Windows 10?
Attention! GCC with the latest C⁺⁺20 fixes is from version 13.0.0.
The GCC version should include the _GLIBCXX_HAS_GTHREADS definitions in c++config. Otherwise, the user will not create the files.
