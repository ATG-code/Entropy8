# Security Policy

## Supported Versions

| Version | Supported |
|---------|-----------|
| Latest `main` | Yes |
| Older releases | Best effort |

## Reporting a Vulnerability

If you discover a security vulnerability in Entropy8, please report it responsibly:

1. **Do NOT open a public GitHub issue** for security vulnerabilities.
2. Contact the maintainers directly via email or a private GitHub Security Advisory.
3. Include a detailed description of the vulnerability, steps to reproduce, and potential impact.
4. Allow reasonable time for the issue to be addressed before any public disclosure.

## Security Model

### Encryption

Entropy8 implements encryption for archive formats that support it:

| Format | Encryption Method | Key Derivation |
|--------|-------------------|----------------|
| E8 (.e8) | AES-256-CBC | PBKDF2-HMAC-SHA256 (100,000 iterations) |
| 7Z (.7z) | AES-256 (via libarchive) | Format-native |
| ZIP (.zip) | AES-256 (via libarchive) | Format-native |

### E8 Encrypted Format Details

- **Key derivation**: PBKDF2-HMAC-SHA256 with 100,000 iterations and a 16-byte random salt
- **Encryption**: AES-256-CBC with PKCS7 padding and a 16-byte random IV
- **Random generation**: Platform-native CSPRNG (`CryptGenRandom` on Windows, `/dev/urandom` on Unix)
- **Format identifier**: `E8AE` magic bytes (vs. `E8A1` for unencrypted)

### What Entropy8 Does NOT Provide

- **Authenticated encryption**: The current E8 format uses AES-256-CBC without an authentication tag (HMAC). Tampered ciphertext may decrypt to garbage rather than being rejected outright. This is a known limitation.
- **Memory protection**: Passwords and derived keys are held in regular process memory. They are not pinned or zeroed on deallocation.
- **Secure deletion**: The "delete after compression" option uses standard file deletion, not secure wiping.

### Dependencies

Entropy8 relies on the following third-party libraries for security-relevant functionality:

- **libarchive** — 7Z and ZIP encryption/decryption
- **Custom AES-256 and SHA-256** — Used for the native E8 format (self-contained, no OpenSSL dependency)

### Best Practices for Users

- Use strong, unique passwords for encrypted archives.
- Prefer E8 or 7Z formats for encryption (AES-256).
- Keep Entropy8 updated to the latest version.
- Do not share encrypted archives and their passwords through the same channel.
