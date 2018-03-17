# Example

```
make
./pack (nix-build '<nixpkgs>' -A firefox) firefox.nar firefox.dep
./unpack /tmp/store firefox.nar firefox.dep 
```

The firefox closure is now unpacked in `/tmp/store` with all paths rewritten accordingly.
Be sure to rename `/nix` to something else before testing to avoid false positives.
