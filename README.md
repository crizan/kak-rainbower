# kak-rainbower
A rainbow highlighter for kakoune \
There are parsers for c/cpp and rust but they are pretty rough so don't expect them to work always well \
If the language is not supported it will use a generic parser which works but does not detect any particular syntax \
The only pairs highlighted are () [] {}, if parsing gets better <> for templates (or generics in rust) should be detectable
# installation
Install with plug.kak or copy the rc folder contents into your kakoune autoload folder \
Compile rainbower.cpp manually and put the binary into the same folder as the rainbow.kak file or use the command rainbower-compile
# modes
rainbow_mode 0 only highlight pairs \
rainbow_mode 1 highlight pairs and current scope in green \
rainbow_mode 2 highlight pairs and scopes in rainbow colors
# used [kak-rainbow](https://github.com/Bodhizafa/kak-rainbow) as a starting point
