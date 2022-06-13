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

The first 4 bytes of this stream is the number of passwords in network byte
order (big endian). Following that is that many `struct password` types. All
numbers are in network byte order.

```
struct string {
	u32 len;
	char data[len + 1]; /* data is '\0' terminated. */
};

struct password {
	u32 headercount;
	struct {
		struct string key;
		struct string value;
	} headers[headercount];
	struct string password;
};
```
