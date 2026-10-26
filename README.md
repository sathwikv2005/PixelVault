# PixelVault

Hide secret messages inside images using DCT-based steganography.

## Usage

**Encode a message**

```powershell
.\PixelVault.exe encode "images\2.jpg" "secret message"
```

Output:

```text
Message embedded successfully
```

**Decode a message**

```powershell
.\PixelVault.exe decode "output\encoded.png"
```

Output:

```text
Decoded message:
secret message
```

## How it works

PixelVault hides messages in an image's frequency coefficients using the Discrete Cosine Transform (DCT).
