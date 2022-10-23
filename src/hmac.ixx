export module hmac;

export void MyFunc();

// https://www.rfc-editor.org/rfc/rfc4231.txt

// hmac-sha256
// msg: what do ya want for nothing?
// key: Jefe
// output: 5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843

// HMAC_SHA256("key", "The quick brown fox jumps over the lazy dog")
//  = f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8
