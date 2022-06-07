# Nate's Password Manager

NPM is a simple dependency free C90 password manager which is probably not
secure at all. It's entirely an exercise to write some encryption algorithms.
Use Bitwarden instead of this.

## Usage

Don't.

## Technical description

NPM uses 256 bit AES encryption with the key being the SHA-256 hash of the
password with 64 bit salts at the end of each block. This allows us to store
arbitrary data in an encrypted form that can be viewed as a stream of bytes.

The first 4 bytes of this stream is the length of the file in network byte
format (big endian). Then comes comes each password, stored in the following
form:

```
struct string {
	uint32_t len;
	char data[len]; /* This is obviously invalid C code */
};

struct password {
	uint32_t headercount;
	struct {
		struct string key;
		struct string value;
	} headers[headercount];
	struct string password;
};
```
