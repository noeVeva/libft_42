/*
 * Selected Libft test runner.
 *
 * This file is included by a generated translation unit that defines only the
 * TEST_FT_* branches requested by the launcher. Each invocation runs exactly
 * one function, so the Python parent can report crashes and timeouts safely.
 */
#include "libft.h"

#include <ctype.h>
#include <dlfcn.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(__GNUC__) || defined(__clang__)
# define UNUSED __attribute__((unused))
#else
# define UNUSED
#endif

static int g_checks;
static int g_failures;

static UNUSED int sign_of(int value)
{
    return ((value > 0) - (value < 0));
}

static UNUSED void report_case(const char *label, const char *reference,
    const char *actual, int pass)
{
    ++g_checks;
    if (!pass)
        ++g_failures;
    printf("[%s] %-22s | original/expected=%-24s | ft=%s\n",
        pass ? "PASS" : "FAIL", label, reference, actual);
}

static UNUSED void report_int(const char *label, long reference, long actual,
    int pass)
{
    char left[64];
    char right[64];

    (void)snprintf(left, sizeof(left), "%ld", reference);
    (void)snprintf(right, sizeof(right), "%ld", actual);
    report_case(label, left, right, pass);
}

static UNUSED void report_string(const char *label, const char *reference,
    const char *actual, int pass)
{
    char left[256];
    char right[256];

    (void)snprintf(left, sizeof(left), "\"%s\"", reference != NULL ? reference : "(null)");
    (void)snprintf(right, sizeof(right), "\"%s\"", actual != NULL ? actual : "(null)");
    report_case(label, left, right, pass);
}

static UNUSED void format_bytes(const unsigned char *data, size_t size,
    char *out, size_t out_size)
{
    size_t index;
    size_t used;
    int written;

    index = 0;
    used = 0;
    while (index < size && used + 4 < out_size)
    {
        written = snprintf(out + used, out_size - used, "%02x%s", data[index],
                index + 1 == size ? "" : " ");
        if (written < 0 || (size_t)written >= out_size - used)
            break;
        used += (size_t)written;
        ++index;
    }
    if (out_size > 0)
        out[out_size - 1] = '\0';
}

static UNUSED void report_bytes(const char *label, const unsigned char *reference,
    const unsigned char *actual, size_t size)
{
    char left[256];
    char right[256];

    format_bytes(reference, size, left, sizeof(left));
    format_bytes(actual, size, right, sizeof(right));
    report_case(label, left, right, memcmp(reference, actual, size) == 0);
}

static UNUSED long pointer_offset(const void *base, size_t size, const void *pointer)
{
    uintptr_t start;
    uintptr_t end;
    uintptr_t address;

    if (pointer == NULL)
        return (-1L);
    start = (uintptr_t)base;
    end = start + size;
    address = (uintptr_t)pointer;
    if (address < start || address > end)
        return (LONG_MIN);
    return ((long)(address - start));
}

static UNUSED void *resolve_symbol(const char *name)
{
    void *handle;
    void *symbol;

    handle = dlopen(NULL, RTLD_NOW);
    if (handle == NULL)
        return (NULL);
    dlerror();
    symbol = dlsym(handle, name);
    (void)dlclose(handle);
    return (symbol);
}

static UNUSED void finish_test(void)
{
    printf("Checks: %d passed, %d failed\n", g_checks - g_failures, g_failures);
}

#ifdef TEST_FT_MEMSET
static void test_ft_memset(void)
{
    unsigned char left[12] = "abcdefghi";
    unsigned char right[12] = "abcdefghi";
    void *left_return;
    void *right_return;
    volatile size_t zero_length = 0;

    left_return = memset(left + 2, 0xa5, 5);
    right_return = ft_memset(right + 2, 0xa5, 5);
    report_bytes("middle fill", left, right, sizeof(left));
    report_int("return offset", pointer_offset(left, sizeof(left), left_return),
        pointer_offset(right, sizeof(right), right_return),
        pointer_offset(left, sizeof(left), left_return) == pointer_offset(right, sizeof(right), right_return));
    (void)memset(left, 'x', zero_length);
    (void)ft_memset(right, 'x', zero_length);
    report_bytes("zero length", left, right, sizeof(left));
    finish_test();
}
#endif

#ifdef TEST_FT_BZERO
static void test_ft_bzero(void)
{
    unsigned char left[10] = "123456789";
    unsigned char right[10] = "123456789";

    bzero(left + 1, 6);
    ft_bzero(right + 1, 6);
    report_bytes("middle zero", left, right, sizeof(left));
    bzero(left, 0);
    ft_bzero(right, 0);
    report_bytes("zero length", left, right, sizeof(left));
    finish_test();
}
#endif

#ifdef TEST_FT_MEMCPY
static void test_ft_memcpy(void)
{
    unsigned char source[8] = {0, 1, 2, 3, 0, 5, 6, 7};
    unsigned char left[10] = {9, 9, 9, 9, 9, 9, 9, 9, 9, 9};
    unsigned char right[10] = {9, 9, 9, 9, 9, 9, 9, 9, 9, 9};
    void *left_return;
    void *right_return;

    left_return = memcpy(left + 1, source, sizeof(source));
    right_return = ft_memcpy(right + 1, source, sizeof(source));
    report_bytes("binary copy", left, right, sizeof(left));
    report_int("return offset", pointer_offset(left, sizeof(left), left_return),
        pointer_offset(right, sizeof(right), right_return),
        pointer_offset(left, sizeof(left), left_return) == pointer_offset(right, sizeof(right), right_return));
    finish_test();
}
#endif

#ifdef TEST_FT_MEMCCPY
static void test_ft_memccpy(void)
{
    const unsigned char source[] = {'a', 'b', 'X', 'd', 'e'};
    unsigned char left[8] = {0};
    unsigned char right[8] = {0};
    void *left_return;
    void *right_return;

    left_return = memccpy(left, source, 'X', sizeof(source));
    right_return = ft_memccpy(right, source, 'X', sizeof(source));
    report_bytes("stop byte copy", left, right, sizeof(left));
    report_int("stop return offset", pointer_offset(left, sizeof(left), left_return),
        pointer_offset(right, sizeof(right), right_return),
        pointer_offset(left, sizeof(left), left_return) == pointer_offset(right, sizeof(right), right_return));
    memset(left, 0xa5, sizeof(left));
    memset(right, 0xa5, sizeof(right));
    left_return = memccpy(left, source, 'Q', sizeof(source));
    right_return = ft_memccpy(right, source, 'Q', sizeof(source));
    report_int("not-found return", left_return == NULL, right_return == NULL,
        (left_return == NULL) == (right_return == NULL));
    report_bytes("not-found full copy", left, right, sizeof(left));
    finish_test();
}
#endif

#ifdef TEST_FT_MEMMOVE
static void test_ft_memmove(void)
{
    unsigned char left[16] = "0123456789";
    unsigned char right[16] = "0123456789";
    void *left_return;
    void *right_return;

    left_return = memmove(left + 2, left, 8);
    right_return = ft_memmove(right + 2, right, 8);
    report_bytes("overlap right", left, right, 12);
    report_int("right return offset", pointer_offset(left, sizeof(left), left_return),
        pointer_offset(right, sizeof(right), right_return),
        left_return == left + 2 && right_return == right + 2);
    left_return = memmove(left, left + 3, 6);
    right_return = ft_memmove(right, right + 3, 6);
    report_bytes("overlap left", left, right, 12);
    report_int("left return offset", pointer_offset(left, sizeof(left), left_return),
        pointer_offset(right, sizeof(right), right_return), left_return == left && right_return == right);
    finish_test();
}
#endif

#ifdef TEST_FT_MEMCHR
static void test_ft_memchr(void)
{
    const unsigned char data[] = {1, 0, 3, 4, 0, 6};
    const void *left;
    const void *right;

    left = memchr(data, 0, sizeof(data));
    right = ft_memchr(data, 0, sizeof(data));
    report_int("embedded zero offset", pointer_offset(data, sizeof(data), left),
        pointer_offset(data, sizeof(data), right), pointer_offset(data, sizeof(data), left) == pointer_offset(data, sizeof(data), right));
    left = memchr(data, 9, sizeof(data));
    right = ft_memchr(data, 9, sizeof(data));
    report_int("missing byte", left == NULL, right == NULL,
        (left == NULL) == (right == NULL));
    finish_test();
}
#endif

#ifdef TEST_FT_MEMCMP
static void test_ft_memcmp(void)
{
    const unsigned char a[] = {0, 20, 255, 4};
    const unsigned char b[] = {0, 20, 1, 4};
    int left;
    int right;

    left = memcmp(a, b, sizeof(a));
    right = ft_memcmp(a, b, sizeof(a));
    report_int("difference sign", sign_of(left), sign_of(right), sign_of(left) == sign_of(right));
    left = memcmp(a, a, sizeof(a));
    right = ft_memcmp(a, a, sizeof(a));
    report_int("equal", left, right, left == right);
    left = memcmp(a, b, 0);
    right = ft_memcmp(a, b, 0);
    report_int("zero length", left, right, left == right);
    finish_test();
}
#endif

#ifdef TEST_FT_STRLEN
static void test_ft_strlen(void)
{
    const char *values[] = {"", "libft", "a string with spaces", "embedded"};
    size_t index;
    size_t left;
    size_t right;
    char label[64];

    index = 0;
    while (index < sizeof(values) / sizeof(values[0]))
    {
        left = strlen(values[index]);
        right = ft_strlen(values[index]);
        (void)snprintf(label, sizeof(label), "case %lu", (unsigned long)index);
        report_int(label, (long)left, (long)right, left == right);
        ++index;
    }
    finish_test();
}
#endif

#ifdef TEST_FT_STRLCPY
static void test_ft_strlcpy(void)
{
    typedef size_t (*fn_type)(char *, const char *, size_t);
    fn_type reference = NULL;
    void *symbol;
    size_t sizes[] = {0, 1, 5, 16};
    size_t index;
    char left[20];
    char right[20];
    size_t left_return;
    size_t right_return;
    char label[64];

    symbol = resolve_symbol("strlcpy");
    if (symbol != NULL)
        memcpy(&reference, &symbol, sizeof(reference));
    index = 0;
    while (index < sizeof(sizes) / sizeof(sizes[0]))
    {
        memset(left, '#', sizeof(left));
        memset(right, '#', sizeof(right));
        right_return = ft_strlcpy(right, "abcdef", sizes[index]);
        if (reference != NULL)
        {
            left_return = reference(left, "abcdef", sizes[index]);
            (void)snprintf(label, sizeof(label), "size %lu bytes", (unsigned long)sizes[index]);
            report_int(label, (long)left_return, (long)right_return,
                left_return == right_return && memcmp(left, right, sizeof(left)) == 0);
        }
        else
        {
            left_return = strlen("abcdef");
            if (sizes[index] > 0)
            {
                size_t copied = sizes[index] - 1 < left_return ? sizes[index] - 1 : left_return;
                memcpy(left, "abcdef", copied);
                left[copied] = '\0';
            }
            (void)snprintf(label, sizeof(label), "size %lu oracle", (unsigned long)sizes[index]);
            report_int(label, (long)left_return, (long)right_return,
                left_return == right_return && memcmp(left, right, sizeof(left)) == 0);
        }
        ++index;
    }
    finish_test();
}
#endif

#ifdef TEST_FT_STRLCAT
static size_t UNUSED oracle_strlcat(char *dst, const char *src, size_t size)
{
    size_t dst_length;
    size_t src_length;
    size_t copied;

    dst_length = strnlen(dst, size);
    src_length = strlen(src);
    if (dst_length == size)
        return (size + src_length);
    copied = 0;
    while (src[copied] != '\0' && dst_length + copied + 1 < size)
    {
        dst[dst_length + copied] = src[copied];
        ++copied;
    }
    if (size > dst_length)
        dst[dst_length + copied] = '\0';
    return (dst_length + src_length);
}

static void test_ft_strlcat(void)
{
    typedef size_t (*fn_type)(char *, const char *, size_t);
    fn_type reference = NULL;
    void *symbol;
    size_t sizes[] = {0, 4, 8, 20};
    size_t index;
    char left[24];
    char right[24];
    size_t left_return;
    size_t right_return;
    char label[64];

    symbol = resolve_symbol("strlcat");
    if (symbol != NULL)
        memcpy(&reference, &symbol, sizeof(reference));
    index = 0;
    while (index < sizeof(sizes) / sizeof(sizes[0]))
    {
        memset(left, '#', sizeof(left));
        memset(right, '#', sizeof(right));
        memcpy(left, "abc\0", 4);
        memcpy(right, "abc\0", 4);
        if (reference != NULL)
            left_return = reference(left, "XYZ", sizes[index]);
        else
            left_return = oracle_strlcat(left, "XYZ", sizes[index]);
        right_return = ft_strlcat(right, "XYZ", sizes[index]);
        (void)snprintf(label, sizeof(label), "size %lu", (unsigned long)sizes[index]);
        report_int(label, (long)left_return, (long)right_return,
            left_return == right_return && memcmp(left, right, sizeof(left)) == 0);
        ++index;
    }
    finish_test();
}
#endif

#ifdef TEST_FT_STRCHR
static void test_ft_strchr(void)
{
    const char *text = "abca";
    int values[] = {'a', 'c', '\0', 'x'};
    size_t index;
    char *left;
    char *right;
    char label[64];

    index = 0;
    while (index < sizeof(values) / sizeof(values[0]))
    {
        left = strchr(text, values[index]);
        right = ft_strchr(text, values[index]);
        (void)snprintf(label, sizeof(label), "byte %d", values[index]);
        report_int(label, pointer_offset(text, strlen(text) + 1, left), pointer_offset(text, strlen(text) + 1, right),
            pointer_offset(text, strlen(text) + 1, left) == pointer_offset(text, strlen(text) + 1, right));
        ++index;
    }
    finish_test();
}
#endif

#ifdef TEST_FT_STRRCHR
static void test_ft_strrchr(void)
{
    const char *text = "abca";
    int values[] = {'a', 'b', '\0', 'x'};
    size_t index;
    char *left;
    char *right;
    char label[64];

    index = 0;
    while (index < sizeof(values) / sizeof(values[0]))
    {
        left = strrchr(text, values[index]);
        right = ft_strrchr(text, values[index]);
        (void)snprintf(label, sizeof(label), "byte %d", values[index]);
        report_int(label, pointer_offset(text, strlen(text) + 1, left), pointer_offset(text, strlen(text) + 1, right),
            pointer_offset(text, strlen(text) + 1, left) == pointer_offset(text, strlen(text) + 1, right));
        ++index;
    }
    finish_test();
}
#endif

#ifdef TEST_FT_STRNSTR
static char *UNUSED oracle_strnstr(const char *haystack, const char *needle, size_t length)
{
    size_t needle_length;
    size_t index;

    needle_length = strlen(needle);
    if (needle_length == 0)
        return ((char *)haystack);
    index = 0;
    while (index + needle_length <= length && haystack[index] != '\0')
    {
        if (strncmp(haystack + index, needle, needle_length) == 0)
            return ((char *)(haystack + index));
        ++index;
    }
    return (NULL);
}

static void test_ft_strnstr(void)
{
    typedef char *(*fn_type)(const char *, const char *, size_t);
    fn_type reference = NULL;
    void *symbol;
    const char *haystack = "hello libft world";
    const char *needles[] = {"libft", "world", "", "missing"};
    size_t lengths[] = {18, 10, 0, 18};
    size_t index;
    char *left;
    char *right;
    char label[64];

    symbol = resolve_symbol("strnstr");
    if (symbol != NULL)
        memcpy(&reference, &symbol, sizeof(reference));
    index = 0;
    while (index < sizeof(needles) / sizeof(needles[0]))
    {
        if (reference != NULL)
            left = reference(haystack, needles[index], lengths[index]);
        else
            left = oracle_strnstr(haystack, needles[index], lengths[index]);
        right = ft_strnstr(haystack, needles[index], lengths[index]);
        (void)snprintf(label, sizeof(label), "needle \"%s\"", needles[index]);
        report_int(label, pointer_offset(haystack, strlen(haystack) + 1, left), pointer_offset(haystack, strlen(haystack) + 1, right),
            pointer_offset(haystack, strlen(haystack) + 1, left) == pointer_offset(haystack, strlen(haystack) + 1, right));
        ++index;
    }
    finish_test();
}
#endif

#ifdef TEST_FT_STRNCMP
static void test_ft_strncmp(void)
{
    const char *a[] = {"abc", "abc", "abz", "\200x"};
    const char *b[] = {"abc", "abd", "aba", "\001x"};
    size_t lengths[] = {3, 2, 3, 1};
    size_t index;
    int left;
    int right;
    char label[64];

    index = 0;
    while (index < sizeof(a) / sizeof(a[0]))
    {
        left = strncmp(a[index], b[index], lengths[index]);
        right = ft_strncmp(a[index], b[index], lengths[index]);
        (void)snprintf(label, sizeof(label), "case %lu", (unsigned long)index);
        report_int(label, sign_of(left), sign_of(right), sign_of(left) == sign_of(right));
        ++index;
    }
    finish_test();
}
#endif

#ifdef TEST_FT_ATOI
static void test_ft_atoi(void)
{
    const char *values[] = {"0", "42", "  \t\n-42xyz", "+00123", "--5", "2147483647", "-2147483648"};
    size_t index;
    int left;
    int right;

    index = 0;
    while (index < sizeof(values) / sizeof(values[0]))
    {
        left = atoi(values[index]);
        right = ft_atoi(values[index]);
        report_int(values[index], left, right, left == right);
        ++index;
    }
    finish_test();
}
#endif

#if defined(TEST_FT_ISALPHA) || defined(TEST_FT_ISDIGIT) || defined(TEST_FT_ISALNUM) \
 || defined(TEST_FT_ISASCII) || defined(TEST_FT_ISPRINT)
static UNUSED void run_predicate(const char *name, int (*reference)(int),
    int (*actual)(int), int include_extended)
{
    int values[] = {EOF, 0, 31, ' ', '0', '9', 'A', 'Z', 'a', 'z', 127, 128, 255};
    size_t count;
    size_t index;
    int left;
    int right;
    char label[64];

    count = sizeof(values) / sizeof(values[0]);
    index = 0;
    while (index < count)
    {
        if (!include_extended && values[index] > 127)
        {
            ++index;
            continue;
        }
        left = reference(values[index]) != 0;
        right = actual(values[index]) != 0;
        (void)snprintf(label, sizeof(label), "%s(%d)", name, values[index]);
        report_int(label, left, right, left == right);
        ++index;
    }
}
#endif

#ifdef TEST_FT_ISALPHA
static void test_ft_isalpha(void)
{
    run_predicate("isalpha", isalpha, ft_isalpha, 1);
    finish_test();
}
#endif
#ifdef TEST_FT_ISDIGIT
static void test_ft_isdigit(void)
{
    run_predicate("isdigit", isdigit, ft_isdigit, 1);
    finish_test();
}
#endif
#ifdef TEST_FT_ISALNUM
static void test_ft_isalnum(void)
{
    run_predicate("isalnum", isalnum, ft_isalnum, 1);
    finish_test();
}
#endif
#ifdef TEST_FT_ISASCII
static void test_ft_isascii(void)
{
    run_predicate("isascii", isascii, ft_isascii, 1);
    finish_test();
}
#endif
#ifdef TEST_FT_ISPRINT
static void test_ft_isprint(void)
{
    run_predicate("isprint", isprint, ft_isprint, 1);
    finish_test();
}
#endif

#ifdef TEST_FT_TOUPPER
static void test_ft_toupper(void)
{
    int values[] = {EOF, 'a', 'z', 'A', '9', ' ', 127, 255};
    size_t index;
    int left;
    int right;
    char label[64];

    index = 0;
    while (index < sizeof(values) / sizeof(values[0]))
    {
        left = toupper(values[index]);
        right = ft_toupper(values[index]);
        (void)snprintf(label, sizeof(label), "toupper(%d)", values[index]);
        report_int(label, left, right, left == right);
        ++index;
    }
    finish_test();
}
#endif

#ifdef TEST_FT_TOLOWER
static void test_ft_tolower(void)
{
    int values[] = {EOF, 'A', 'Z', 'a', '9', ' ', 127, 255};
    size_t index;
    int left;
    int right;
    char label[64];

    index = 0;
    while (index < sizeof(values) / sizeof(values[0]))
    {
        left = tolower(values[index]);
        right = ft_tolower(values[index]);
        (void)snprintf(label, sizeof(label), "tolower(%d)", values[index]);
        report_int(label, left, right, left == right);
        ++index;
    }
    finish_test();
}
#endif

#ifdef TEST_FT_CALLOC
static void test_ft_calloc(void)
{
    unsigned char *left;
    unsigned char *right;
    unsigned char zeros[32] = {0};

    left = calloc(8, 4);
    right = ft_calloc(8, 4);
    report_case("allocation", left != NULL ? "non-null" : "null",
        right != NULL ? "non-null" : "null", (left == NULL) == (right == NULL));
    if (left != NULL && right != NULL)
        report_bytes("zero initialized", zeros, right, sizeof(zeros));
    free(left);
    free(right);
    right = ft_calloc(0, 16);
    report_case("zero count", "valid: NULL or freeable", right == NULL ? "null" : "freeable",
        1);
    free(right);
    finish_test();
}
#endif

#ifdef TEST_FT_STRDUP
static void test_ft_strdup(void)
{
    const char *values[] = {"", "libft", "spaces and symbols !@#"};
    size_t index;
    char *left;
    char *right;

    index = 0;
    while (index < sizeof(values) / sizeof(values[0]))
    {
        left = strdup(values[index]);
        right = ft_strdup(values[index]);
        report_string(values[index], left, right,
            left != NULL && right != NULL && strcmp(left, right) == 0 && right != values[index]);
        free(left);
        free(right);
        ++index;
    }
    finish_test();
}
#endif

#ifdef TEST_FT_SUBSTR
static void test_ft_substr(void)
{
    char *result;

    result = ft_substr("hello world", 6, 5);
    report_string("normal", "world", result, result != NULL && strcmp(result, "world") == 0);
    free(result);
    result = ft_substr("abc", 99, 4);
    report_string("start beyond end", "", result, result != NULL && strcmp(result, "") == 0);
    free(result);
    result = ft_substr("abc", 1, 0);
    report_string("zero length", "", result, result != NULL && strcmp(result, "") == 0);
    free(result);
    finish_test();
}
#endif

#ifdef TEST_FT_STRJOIN
static void test_ft_strjoin(void)
{
    char *result;

    result = ft_strjoin("hello ", "libft");
    report_string("normal", "hello libft", result,
        result != NULL && strcmp(result, "hello libft") == 0);
    free(result);
    result = ft_strjoin("", "");
    report_string("empty", "", result, result != NULL && strcmp(result, "") == 0);
    free(result);
    finish_test();
}
#endif

#ifdef TEST_FT_STRTRIM
static void test_ft_strtrim(void)
{
    char *result;

    result = ft_strtrim("xx hello xy", "xy ");
    report_string("both ends", "hello", result, result != NULL && strcmp(result, "hello") == 0);
    free(result);
    result = ft_strtrim("aaaa", "a");
    report_string("all trimmed", "", result, result != NULL && strcmp(result, "") == 0);
    free(result);
    result = ft_strtrim("libft", "");
    report_string("empty set", "libft", result, result != NULL && strcmp(result, "libft") == 0);
    free(result);
    finish_test();
}
#endif

#ifdef TEST_FT_SPLIT
static void UNUSED free_split(char **items)
{
    size_t index;

    if (items == NULL)
        return;
    index = 0;
    while (items[index] != NULL)
    {
        free(items[index]);
        ++index;
    }
    free(items);
}

static int UNUSED split_equals(char **items, const char **expected)
{
    size_t index;

    if (items == NULL)
        return (0);
    index = 0;
    while (expected[index] != NULL && items[index] != NULL)
    {
        if (strcmp(items[index], expected[index]) != 0)
            return (0);
        ++index;
    }
    return (expected[index] == NULL && items[index] == NULL);
}

static void test_ft_split(void)
{
    const char *expected_one[] = {"alpha", "beta", "gamma", NULL};
    const char *expected_empty[] = {NULL};
    char **result;

    result = ft_split(",,alpha,beta,,,gamma,", ',');
    report_case("repeated delimiter", "[alpha,beta,gamma]",
        split_equals(result, expected_one) ? "[alpha,beta,gamma]" : "different/null",
        split_equals(result, expected_one));
    free_split(result);
    result = ft_split("::::", ':');
    report_case("only delimiters", "[]", split_equals(result, expected_empty) ? "[]" : "different/null",
        split_equals(result, expected_empty));
    free_split(result);
    finish_test();
}
#endif

#ifdef TEST_FT_ITOA
static void test_ft_itoa(void)
{
    int values[] = {0, 42, -42, INT_MAX, INT_MIN};
    size_t index;
    char expected[64];
    char *result;

    index = 0;
    while (index < sizeof(values) / sizeof(values[0]))
    {
        (void)snprintf(expected, sizeof(expected), "%d", values[index]);
        result = ft_itoa(values[index]);
        report_string(expected, expected, result, result != NULL && strcmp(result, expected) == 0);
        free(result);
        ++index;
    }
    finish_test();
}
#endif

#ifdef TEST_FT_STRMAPI
static char UNUSED map_character(unsigned int index, char character)
{
    return ((char)(character + (char)(index % 3)));
}

static void test_ft_strmapi(void)
{
    char *result;

    result = ft_strmapi("abcd", map_character);
    report_string("indexed mapping", "aced", result, result != NULL && strcmp(result, "aced") == 0);
    free(result);
    result = ft_strmapi("", map_character);
    report_string("empty", "", result, result != NULL && strcmp(result, "") == 0);
    free(result);
    finish_test();
}
#endif

#if defined(TEST_FT_PUTCHAR_FD) || defined(TEST_FT_PUTSTR_FD) \
 || defined(TEST_FT_PUTENDL_FD) || defined(TEST_FT_PUTNBR_FD)
typedef void (*writer_fn)(int fd);

static UNUSED int capture_writer(writer_fn writer, char *output, size_t capacity)
{
    int descriptors[2];
    ssize_t count;

    if (capacity == 0 || pipe(descriptors) != 0)
        return (-1);
    writer(descriptors[1]);
    if (close(descriptors[1]) != 0)
    {
        (void)close(descriptors[0]);
        return (-1);
    }
    count = read(descriptors[0], output, capacity - 1);
    (void)close(descriptors[0]);
    if (count < 0)
        return (-1);
    output[count] = '\0';
    return ((int)count);
}
#endif

#ifdef TEST_FT_PUTCHAR_FD
static void UNUSED write_putchar_case(int fd) { ft_putchar_fd('Z', fd); }
static void test_ft_putchar_fd(void)
{
    char output[32];
    int count = capture_writer(write_putchar_case, output, sizeof(output));
    report_string("pipe capture", "Z", count >= 0 ? output : "capture-error",
        count == 1 && strcmp(output, "Z") == 0);
    finish_test();
}
#endif

#ifdef TEST_FT_PUTSTR_FD
static void UNUSED write_putstr_case(int fd) { ft_putstr_fd("hello", fd); }
static void test_ft_putstr_fd(void)
{
    char output[32];
    int count = capture_writer(write_putstr_case, output, sizeof(output));
    report_string("pipe capture", "hello", count >= 0 ? output : "capture-error",
        count == 5 && strcmp(output, "hello") == 0);
    finish_test();
}
#endif

#ifdef TEST_FT_PUTENDL_FD
static void UNUSED write_putendl_case(int fd) { ft_putendl_fd("hello", fd); }
static void test_ft_putendl_fd(void)
{
    char output[32];
    int count = capture_writer(write_putendl_case, output, sizeof(output));
    report_string("pipe capture", "hello\n", count >= 0 ? output : "capture-error",
        count == 6 && strcmp(output, "hello\n") == 0);
    finish_test();
}
#endif

#ifdef TEST_FT_PUTNBR_FD
static void UNUSED write_putnbr_case(int fd) { ft_putnbr_fd(INT_MIN, fd); }
static void test_ft_putnbr_fd(void)
{
    char output[64];
    char expected[64];
    int count;

    (void)snprintf(expected, sizeof(expected), "%d", INT_MIN);
    count = capture_writer(write_putnbr_case, output, sizeof(output));
    report_string("INT_MIN pipe", expected, count >= 0 ? output : "capture-error",
        count == (int)strlen(expected) && strcmp(output, expected) == 0);
    finish_test();
}
#endif

#if defined(TEST_FT_LSTNEW) || defined(TEST_FT_LSTADD_FRONT) || defined(TEST_FT_LSTSIZE) \
 || defined(TEST_FT_LSTLAST) || defined(TEST_FT_LSTADD_BACK) || defined(TEST_FT_LSTDELONE) \
 || defined(TEST_FT_LSTCLEAR) || defined(TEST_FT_LSTITER) || defined(TEST_FT_LSTMAP)
static int g_deleted;

static UNUSED void delete_int(void *content)
{
    ++g_deleted;
    free(content);
}

static UNUSED int *new_int(int value)
{
    int *pointer;

    pointer = malloc(sizeof(*pointer));
    if (pointer != NULL)
        *pointer = value;
    return (pointer);
}

static UNUSED void free_nodes_plain(t_list *list)
{
    t_list *next;

    while (list != NULL)
    {
        next = list->next;
        free(list);
        list = next;
    }
}

static UNUSED t_list *make_int_node(int value)
{
    t_list *node;

    node = malloc(sizeof(*node));
    if (node == NULL)
        return (NULL);
    node->content = new_int(value);
    node->next = NULL;
    if (node->content == NULL)
    {
        free(node);
        return (NULL);
    }
    return (node);
}

static UNUSED void destroy_int_nodes(t_list *list)
{
    t_list *next;

    while (list != NULL)
    {
        next = list->next;
        free(list->content);
        free(list);
        list = next;
    }
}
#endif

#ifdef TEST_FT_LSTNEW
static void test_ft_lstnew(void)
{
    int value = 42;
    t_list *node = ft_lstnew(&value);

    report_case("allocation", "node", node != NULL ? "node" : "null", node != NULL);
    if (node != NULL)
    {
        report_case("content pointer", "same", node->content == &value ? "same" : "different",
            node->content == &value);
        report_case("next initialized", "null", node->next == NULL ? "null" : "non-null",
            node->next == NULL);
    }
    free(node);
    finish_test();
}
#endif

#ifdef TEST_FT_LSTADD_FRONT
static void test_ft_lstadd_front(void)
{
    t_list first;
    t_list added;
    t_list *head = &first;

    first.content = NULL;
    first.next = NULL;
    added.content = NULL;
    added.next = NULL;
    ft_lstadd_front(&head, &added);
    report_case("new head", "added", head == &added ? "added" : "different", head == &added);
    report_case("old head linked", "first", added.next == &first ? "first" : "different",
        added.next == &first);
    finish_test();
}
#endif

#ifdef TEST_FT_LSTSIZE
static void test_ft_lstsize(void)
{
    t_list nodes[3];

    nodes[0].next = &nodes[1];
    nodes[1].next = &nodes[2];
    nodes[2].next = NULL;
    report_int("three nodes", 3, ft_lstsize(&nodes[0]), ft_lstsize(&nodes[0]) == 3);
    report_int("empty", 0, ft_lstsize(NULL), ft_lstsize(NULL) == 0);
    finish_test();
}
#endif

#ifdef TEST_FT_LSTLAST
static void test_ft_lstlast(void)
{
    t_list nodes[3];
    t_list *result;

    nodes[0].next = &nodes[1];
    nodes[1].next = &nodes[2];
    nodes[2].next = NULL;
    result = ft_lstlast(&nodes[0]);
    report_case("three nodes", "third", result == &nodes[2] ? "third" : "different",
        result == &nodes[2]);
    result = ft_lstlast(NULL);
    report_case("empty", "null", result == NULL ? "null" : "non-null", result == NULL);
    finish_test();
}
#endif

#ifdef TEST_FT_LSTADD_BACK
static void test_ft_lstadd_back(void)
{
    t_list first;
    t_list added;
    t_list *head = &first;

    first.content = NULL;
    first.next = NULL;
    added.content = NULL;
    added.next = NULL;
    ft_lstadd_back(&head, &added);
    report_case("append", "first->added", first.next == &added ? "first->added" : "different",
        first.next == &added);
    head = NULL;
    added.next = NULL;
    ft_lstadd_back(&head, &added);
    report_case("append to empty", "new head", head == &added ? "new head" : "different",
        head == &added);
    finish_test();
}
#endif

#ifdef TEST_FT_LSTDELONE
static void test_ft_lstdelone(void)
{
    t_list *node = malloc(sizeof(*node));

    if (node == NULL)
    {
        report_case("fixture allocation", "success", "failed", 0);
        finish_test();
        return;
    }
    node->content = new_int(7);
    node->next = NULL;
    g_deleted = 0;
    ft_lstdelone(node, delete_int);
    report_int("deleter calls", 1, g_deleted, g_deleted == 1);
    finish_test();
}
#endif

#ifdef TEST_FT_LSTCLEAR
static void test_ft_lstclear(void)
{
    t_list *a = make_int_node(1);
    t_list *b = make_int_node(2);
    t_list *c = make_int_node(3);

    if (a == NULL || b == NULL || c == NULL)
    {
        report_case("fixture allocation", "success", "failed", 0);
        destroy_int_nodes(a);
        destroy_int_nodes(b);
        destroy_int_nodes(c);
        finish_test();
        return;
    }
    a->next = b;
    b->next = c;
    g_deleted = 0;
    ft_lstclear(&a, delete_int);
    report_int("deleter calls", 3, g_deleted, g_deleted == 3);
    report_case("head reset", "null", a == NULL ? "null" : "non-null", a == NULL);
    finish_test();
}
#endif

#ifdef TEST_FT_LSTITER
static void UNUSED increment_int(void *content)
{
    ++*(int *)content;
}

static void test_ft_lstiter(void)
{
    int values[] = {1, 2, 3};
    t_list nodes[3];

    nodes[0].content = &values[0];
    nodes[0].next = &nodes[1];
    nodes[1].content = &values[1];
    nodes[1].next = &nodes[2];
    nodes[2].content = &values[2];
    nodes[2].next = NULL;
    ft_lstiter(&nodes[0], increment_int);
    report_case("mapped values", "[2,3,4]",
        values[0] == 2 && values[1] == 3 && values[2] == 4 ? "[2,3,4]" : "different",
        values[0] == 2 && values[1] == 3 && values[2] == 4);
    finish_test();
}
#endif

#ifdef TEST_FT_LSTMAP
static void *UNUSED double_int(void *content)
{
    int *result = new_int(*(int *)content * 2);
    return (result);
}

static void test_ft_lstmap(void)
{
    int values[] = {2, 4, 6};
    t_list nodes[3];
    t_list *mapped;
    t_list *cursor;
    int expected[] = {4, 8, 12};
    int valid;
    size_t index;

    nodes[0].content = &values[0];
    nodes[0].next = &nodes[1];
    nodes[1].content = &values[1];
    nodes[1].next = &nodes[2];
    nodes[2].content = &values[2];
    nodes[2].next = NULL;
    g_deleted = 0;
    mapped = ft_lstmap(&nodes[0], double_int, delete_int);
    valid = mapped != NULL;
    cursor = mapped;
    index = 0;
    while (valid && cursor != NULL && index < 3)
    {
        valid = cursor->content != NULL && *(int *)cursor->content == expected[index]
            && cursor->content != nodes[index].content;
        cursor = cursor->next;
        ++index;
    }
    valid = valid && index == 3 && cursor == NULL;
    report_case("mapped copy", "[4,8,12] distinct", valid ? "[4,8,12] distinct" : "different/null", valid);
    destroy_int_nodes(mapped);
    finish_test();
}
#endif

struct test_entry
{
    const char *name;
    void (*run)(void);
};

static const struct test_entry g_tests[] = {
#ifdef TEST_FT_MEMSET
    {"ft_memset", test_ft_memset},
#endif
#ifdef TEST_FT_BZERO
    {"ft_bzero", test_ft_bzero},
#endif
#ifdef TEST_FT_MEMCPY
    {"ft_memcpy", test_ft_memcpy},
#endif
#ifdef TEST_FT_MEMCCPY
    {"ft_memccpy", test_ft_memccpy},
#endif
#ifdef TEST_FT_MEMMOVE
    {"ft_memmove", test_ft_memmove},
#endif
#ifdef TEST_FT_MEMCHR
    {"ft_memchr", test_ft_memchr},
#endif
#ifdef TEST_FT_MEMCMP
    {"ft_memcmp", test_ft_memcmp},
#endif
#ifdef TEST_FT_STRLEN
    {"ft_strlen", test_ft_strlen},
#endif
#ifdef TEST_FT_STRLCPY
    {"ft_strlcpy", test_ft_strlcpy},
#endif
#ifdef TEST_FT_STRLCAT
    {"ft_strlcat", test_ft_strlcat},
#endif
#ifdef TEST_FT_STRCHR
    {"ft_strchr", test_ft_strchr},
#endif
#ifdef TEST_FT_STRRCHR
    {"ft_strrchr", test_ft_strrchr},
#endif
#ifdef TEST_FT_STRNSTR
    {"ft_strnstr", test_ft_strnstr},
#endif
#ifdef TEST_FT_STRNCMP
    {"ft_strncmp", test_ft_strncmp},
#endif
#ifdef TEST_FT_ATOI
    {"ft_atoi", test_ft_atoi},
#endif
#ifdef TEST_FT_ISALPHA
    {"ft_isalpha", test_ft_isalpha},
#endif
#ifdef TEST_FT_ISDIGIT
    {"ft_isdigit", test_ft_isdigit},
#endif
#ifdef TEST_FT_ISALNUM
    {"ft_isalnum", test_ft_isalnum},
#endif
#ifdef TEST_FT_ISASCII
    {"ft_isascii", test_ft_isascii},
#endif
#ifdef TEST_FT_ISPRINT
    {"ft_isprint", test_ft_isprint},
#endif
#ifdef TEST_FT_TOUPPER
    {"ft_toupper", test_ft_toupper},
#endif
#ifdef TEST_FT_TOLOWER
    {"ft_tolower", test_ft_tolower},
#endif
#ifdef TEST_FT_CALLOC
    {"ft_calloc", test_ft_calloc},
#endif
#ifdef TEST_FT_STRDUP
    {"ft_strdup", test_ft_strdup},
#endif
#ifdef TEST_FT_SUBSTR
    {"ft_substr", test_ft_substr},
#endif
#ifdef TEST_FT_STRJOIN
    {"ft_strjoin", test_ft_strjoin},
#endif
#ifdef TEST_FT_STRTRIM
    {"ft_strtrim", test_ft_strtrim},
#endif
#ifdef TEST_FT_SPLIT
    {"ft_split", test_ft_split},
#endif
#ifdef TEST_FT_ITOA
    {"ft_itoa", test_ft_itoa},
#endif
#ifdef TEST_FT_STRMAPI
    {"ft_strmapi", test_ft_strmapi},
#endif
#ifdef TEST_FT_PUTCHAR_FD
    {"ft_putchar_fd", test_ft_putchar_fd},
#endif
#ifdef TEST_FT_PUTSTR_FD
    {"ft_putstr_fd", test_ft_putstr_fd},
#endif
#ifdef TEST_FT_PUTENDL_FD
    {"ft_putendl_fd", test_ft_putendl_fd},
#endif
#ifdef TEST_FT_PUTNBR_FD
    {"ft_putnbr_fd", test_ft_putnbr_fd},
#endif
#ifdef TEST_FT_LSTNEW
    {"ft_lstnew", test_ft_lstnew},
#endif
#ifdef TEST_FT_LSTADD_FRONT
    {"ft_lstadd_front", test_ft_lstadd_front},
#endif
#ifdef TEST_FT_LSTSIZE
    {"ft_lstsize", test_ft_lstsize},
#endif
#ifdef TEST_FT_LSTLAST
    {"ft_lstlast", test_ft_lstlast},
#endif
#ifdef TEST_FT_LSTADD_BACK
    {"ft_lstadd_back", test_ft_lstadd_back},
#endif
#ifdef TEST_FT_LSTDELONE
    {"ft_lstdelone", test_ft_lstdelone},
#endif
#ifdef TEST_FT_LSTCLEAR
    {"ft_lstclear", test_ft_lstclear},
#endif
#ifdef TEST_FT_LSTITER
    {"ft_lstiter", test_ft_lstiter},
#endif
#ifdef TEST_FT_LSTMAP
    {"ft_lstmap", test_ft_lstmap},
#endif
};

int main(int argc, char **argv)
{
    size_t index;

    if (argc != 2)
    {
        fprintf(stderr, "generated runner expects exactly one function name\n");
        return (2);
    }
    index = 0;
    while (index < sizeof(g_tests) / sizeof(g_tests[0]))
    {
        if (strcmp(argv[1], g_tests[index].name) == 0)
        {
            g_tests[index].run();
            return (g_failures == 0 ? 0 : 1);
        }
        ++index;
    }
    fprintf(stderr, "function was not compiled into this runner: %s\n", argv[1]);
    return (2);
}
