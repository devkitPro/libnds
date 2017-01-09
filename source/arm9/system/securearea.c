/*
	Array used to fill secure area, marked weak to allow nds files to be
	built with no secure area.

	To disable this add 'const int __secure_area__ = 0;'

	Value and type are unimportant the symbol only needs to exist
	elsewhere to prevent this one being linked.

*/

__attribute__((section(".secure")))
__attribute__((weak))
const char __secure_area__[2048];
