# kak-rainbower
A rainbow highlighter for kakoune \
There are parsers for c/cpp and rust but they are pretty rough so don't expect them to work always well \
If the language is not supported it will use a generic parser which works but does not detect any particular syntax \
The only pairs highlighted are () [] {}, if parsing gets better <> for templates (or generics in rust) should be detectable
# installation
You will need to compile rainbower.cpp as rainbower and put it somewhere in your PATH \
Then add the content of the rc directory inside your kakoune autoload directory or use plug.kak
# modes
rainbow_mode 0 only highlight pairs \
rainbow_mode 1 highlight pairs and current scope in green \
rainbow_mode 2 highlight pairs and scopes in rainbow colors
# used [kak-rainbow](https://github.com/Bodhizafa/kak-rainbow) as a starting point
