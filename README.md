# Nate's Password Manager

NPM is a simple dependency free C90 password manager which is probably not
secure at all. It's entirely an exercise to write some encryption algorithms.
If you need a password manager (which you do), use Bitwarden instead of NPM.

NPM's passwords have public and private properties. Public properties are
properties which aren't detrimental to security if they're released. Things like
a username or a domain, where if someone looks over your shoulder and sees them,
you're fine. Private properties, on the other hand, are detrimental to security
if released, things like passwords and credit card information. Every property
is made up of a name and a value, which are defined by the user.

## Usage

```
npm add
    -m [password]        Set the master password
    -p [name:value]      Create a public property
    -s [name:value]      Create a private (secret) property
    -f [file]            Set the file that NPM looks for passwords in
                             (default: ~/.local/share/npm)

npm update [old master password] [new master password]
    -f [file]            Set the file for NPM to change the master password for

npm get
    -m [password]        Set the master password
    -p [name:value]      Only get passwords with this public property
    -s [name:value]      Only get passwords with this private property
    -f [file]            Set the file for NPM to get passwords from

npm del
    -m [password]        Set the master password
    -p [name:value]      Only delete passwords with this public property
    -f [file]            Set the file for NPM to delete passwords from
```

## Technical description

NPM uses 256 bit AES encryption with the key being the SHA-256 hash of the
password with 64 bit salts at the end of each block. This allows us to store
arbitrary data in an encrypted form that can be viewed as a stream of bytes.

The stream can be parsed using the `struct file` defined below. All integers
are in network byte order (big endian)

```
struct file {
	char magic[4]; /* Always "NPM\x00" */
	u32 passwordcount;
	struct password passwords[passwordcount];
};

struct password {
	u32 publiccount;
	struct property public[publiccount];
	u32 privatecount;
	struct property private[privatecount];
};

struct property {
	struct string name;
	struct string value;
};

struct string {
	u32 len;
	char data[len + 1]; /* data is '\0' terminated */
};
```
