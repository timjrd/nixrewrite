Prototype implementation of Nix store path rewriting as described in [this issue](https://github.com/NixOS/nix/issues/1971).

# Example

```
make
./pack $(nix-build '<nixpkgs>' -A firefox) firefox.cpio firefox.dep
./unpack /tmp/store firefox.cpio firefox.dep 
```

The firefox closure is now unpacked in `/tmp/store` with all paths rewritten accordingly.
Be sure to rename `/nix` to something else before testing to avoid false positives.
