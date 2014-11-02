## Types

```cpp
template <typename T, typename U>
struct exapand_utf : std::integral_constant<std::size_t, /* see table */> 
```

### Template Parameters

* `T` is an 8, 16, or 32 bit integral type for a UTF code point
* `U` is an 8, 16, or 32 bit integral type for a UTF code point

### Members
    
* `value` is the maximum expansion when converting a UTF 
sequence from `T` to `U` determined by the following table:

|source/result|UTF-8|UTF-16|UTF-32|
|------------:|:---:|:----:|:----:|
|UTF-8        |  1  |   1  |   1  |
|UTF-16       |  3  |   1  |   1  |
|UTF-32       |  4  |   2  |   1  |

_see_ [`copy_utf`](#copy-utf)

## Functions

<a name="copy-utf"></a>

```cpp
template <typename T, typename I, typename O>
O copy_utf(I f, I l, O o);

template <typename T, typename R, typename O>
O copy_utf(const R& r, O o);
```

`copy_utf` copies the text from the range `[f, l)` from UTF-8, 16, or 32 to UTF-8, 16
or 32 and assigns the result to `*o`.

### Template Parameters

* `T` must be 8, 16, or 32 bit integral type
* `I` models `InputIterator`; `value_type(I)` must be 8, 16, or 32 bit integral type
* `O` models `OutputIterator`; must accept `T`

### Preconditions
* `[f, l)` is a valid range of UTF-8, 16, or 32 encode text
* `o` is not an iterator within the range `[f, l)`
* There is enough space to hold the text being copied. The maximum
  requirement on the output is that `[o, o + m(l - f))` is a valid range
  where `m` is `expand_utf<value_type(I), T>::value`

#### _Note_
If the source contains an invalid or partial encoding then the output is undefined (debug
builds may assert). However, the code will not read beyond the specified source range or output
more than the maximum number of elements.

### Return
An output iterator pointing to the end of the encoded text.

