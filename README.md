# kak-rainbower
A rainbow highlighter for kakoune \
There are parsers for c/cpp and rust but they are pretty rough so don't expect them to work always well \
If the language is not supported it will use a generic parser which works but does not detect any particular syntax \
Currently highlights () [] {}, <> only in cpp and rust with rainbow_check_templates set to Y
# installation
Install with plug.kak or copy the rc folder contents into your kakoune autoload folder \
Compile rainbower.cpp manually (for example: `g++ rainbower.cpp -O2 -o rainbower`). The binary needs to be in the same folder as the rainbow.kak file. Or use the command rainbower-compile (requires gcc or clang installed)
# modes
rainbow_mode 0 only highlight pairs \
rainbow_mode 1 highlight pairs and current scope in green \
rainbow_mode 2 highlight pairs and scopes in rainbow colors
# used [kak-rainbow](https://github.com/Bodhizafa/kak-rainbow) as a starting point
