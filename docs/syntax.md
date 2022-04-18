# Love syntax

## Level switching
```love
void main() {
    int x = 2;
    int y = 2;

    <~
        var z = 2;
    >
    -> z int;
    printf("%d\n", z); // 2;
}
```

* low-level mode = `ll`
* high-level mode = `hl`
