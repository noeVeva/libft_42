#!/usr/bin/env python3
"""Compiler-driven test launcher for the 42 Libft project.

The launcher discovers known ft_* definitions, computes a conservative source
closure, generates a selected C dispatcher, compiles it with strict warnings,
and executes every selected function in its own process.
"""

from __future__ import annotations

import argparse
import ctypes
import os
import re
import shlex
import shutil
import signal
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


@dataclass(frozen=True)
class TestSpec:
    name: str
    reference: str | None
    group: str


SPECS = [
    TestSpec("ft_memset", "memset", "libc-memory"),
    TestSpec("ft_bzero", "bzero", "libc-memory"),
    TestSpec("ft_memcpy", "memcpy", "libc-memory"),
    TestSpec("ft_memccpy", "memccpy", "libc-memory"),
    TestSpec("ft_memmove", "memmove", "libc-memory"),
    TestSpec("ft_memchr", "memchr", "libc-memory"),
    TestSpec("ft_memcmp", "memcmp", "libc-memory"),
    TestSpec("ft_strlen", "strlen", "libc-string"),
    TestSpec("ft_strlcpy", "strlcpy", "bsd-string"),
    TestSpec("ft_strlcat", "strlcat", "bsd-string"),
    TestSpec("ft_strchr", "strchr", "libc-string"),
    TestSpec("ft_strrchr", "strrchr", "libc-string"),
    TestSpec("ft_strnstr", "strnstr", "bsd-string"),
    TestSpec("ft_strncmp", "strncmp", "libc-string"),
    TestSpec("ft_atoi", "atoi", "libc-conversion"),
    TestSpec("ft_isalpha", "isalpha", "libc-ctype"),
    TestSpec("ft_isdigit", "isdigit", "libc-ctype"),
    TestSpec("ft_isalnum", "isalnum", "libc-ctype"),
    TestSpec("ft_isascii", "isascii", "libc-ctype"),
    TestSpec("ft_isprint", "isprint", "libc-ctype"),
    TestSpec("ft_toupper", "toupper", "libc-ctype"),
    TestSpec("ft_tolower", "tolower", "libc-ctype"),
    TestSpec("ft_calloc", "calloc", "libc-allocation"),
    TestSpec("ft_strdup", "strdup", "libc-allocation"),
    TestSpec("ft_substr", None, "additional"),
    TestSpec("ft_strjoin", None, "additional"),
    TestSpec("ft_strtrim", None, "additional"),
    TestSpec("ft_split", None, "additional"),
    TestSpec("ft_itoa", None, "additional"),
    TestSpec("ft_strmapi", None, "additional"),
    TestSpec("ft_putchar_fd", None, "additional-fd"),
    TestSpec("ft_putstr_fd", None, "additional-fd"),
    TestSpec("ft_putendl_fd", None, "additional-fd"),
    TestSpec("ft_putnbr_fd", None, "additional-fd"),
    TestSpec("ft_lstnew", None, "bonus-list"),
    TestSpec("ft_lstadd_front", None, "bonus-list"),
    TestSpec("ft_lstsize", None, "bonus-list"),
    TestSpec("ft_lstlast", None, "bonus-list"),
    TestSpec("ft_lstadd_back", None, "bonus-list"),
    TestSpec("ft_lstdelone", None, "bonus-list"),
    TestSpec("ft_lstclear", None, "bonus-list"),
    TestSpec("ft_lstiter", None, "bonus-list"),
    TestSpec("ft_lstmap", None, "bonus-list"),
]
SPEC_BY_NAME = {spec.name: spec for spec in SPECS}
FUNCTION_PATTERN = re.compile(r"\b(ft_[A-Za-z0-9_]+)\s*\(")
DEFINITION_PATTERN = re.compile(
    r"(?:^|\n)\s*(?:[A-Za-z_][A-Za-z0-9_]*\s+|\*\s*)+"
    r"(ft_[A-Za-z0-9_]+)\s*\([^;{}]*\)\s*\{",
    re.MULTILINE,
)


@dataclass
class Discovery:
    source_by_function: dict[str, Path]
    calls_by_function: dict[str, set[str]]
    duplicates: dict[str, list[Path]]
    extra_functions: set[str]
    all_source_paths: list[Path]


def default_compiler() -> str:
    configured = os.environ.get("CC")
    if configured:
        return configured
    for candidate in ("cc", "gcc", "clang"):
        located = shutil.which(candidate)
        if located:
            return located
    conda_compiler = Path.home() / "micromamba" / "bin" / "x86_64-conda-linux-gnu-cc"
    if conda_compiler.is_file():
        return str(conda_compiler)
    return "cc"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        prog="libft-test",
        description="Discover, compile, and isolate tests for 42 Libft functions.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=(
            "examples:\n"
            "  ./libft-test --libft ../libft --list\n"
            "  ./libft-test --libft ../libft ft_strlen\n"
            "  ./libft-test --libft ../libft strlen ft_substr\n"
            "  ./libft-test --libft ../libft --all\n"
        ),
    )
    parser.add_argument("functions", nargs="*", help="function names, with or without ft_ (commas accepted)")
    parser.add_argument("--libft", type=Path, default=Path.cwd(), help="path to the Libft source directory")
    parser.add_argument("--list", action="store_true", help="list supported tests and discovered availability")
    parser.add_argument("--all", action="store_true", help="test every supported function found in the directory")
    parser.add_argument("--cc", default=default_compiler(), help="C compiler (default: $CC, cc, gcc, or clang)")
    parser.add_argument("--cflag", action="append", default=[], help="extra compiler flag; repeat as needed")
    parser.add_argument("--timeout", type=float, default=5.0, help="seconds allowed per function (default: 5)")
    parser.add_argument("--keep-build", action="store_true", help="preserve the generated build directory")
    parser.add_argument("--verbose", action="store_true", help="show source closure and commands")
    parser.add_argument(
        "--all-sources",
        action="store_true",
        help="compile every discovered ft_ source instead of the dependency closure",
    )
    parser.add_argument("--self-check", action="store_true", help="compile all test branches without linking")
    return parser.parse_args()


def read_source(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError as exc:
        raise RuntimeError(f"cannot read {path}: {exc}") from exc


def mask_c_comments_and_literals(text: str) -> str:
    """Replace comments and literal contents with spaces while preserving lines."""
    output = list(text)
    index = 0
    state = "code"
    while index < len(text):
        current = text[index]
        following = text[index + 1] if index + 1 < len(text) else ""
        if state == "code":
            if current == "/" and following == "/":
                output[index] = output[index + 1] = " "
                index += 2
                state = "line-comment"
                continue
            if current == "/" and following == "*":
                output[index] = output[index + 1] = " "
                index += 2
                state = "block-comment"
                continue
            if current == '"':
                output[index] = " "
                state = "string"
            elif current == "'":
                output[index] = " "
                state = "character"
        elif state == "line-comment":
            if current == "\n":
                state = "code"
            else:
                output[index] = " "
        elif state == "block-comment":
            if current == "*" and following == "/":
                output[index] = output[index + 1] = " "
                index += 2
                state = "code"
                continue
            if current != "\n":
                output[index] = " "
        elif state in {"string", "character"}:
            quote = '"' if state == "string" else "'"
            if current == "\\" and following:
                output[index] = " "
                if following != "\n":
                    output[index + 1] = " "
                index += 2
                continue
            if current == quote:
                output[index] = " "
                state = "code"
            elif current != "\n":
                output[index] = " "
        index += 1
    return "".join(output)


def relative_is_ignored(path: Path, root: Path) -> bool:
    ignored_parts = {".git", ".libft-tester-build", "libft-tester", "build"}
    return any(part in ignored_parts for part in path.relative_to(root).parts[:-1])


def discover(libft_dir: Path) -> Discovery:
    source_by_function: dict[str, Path] = {}
    calls_by_function: dict[str, set[str]] = {}
    duplicates: dict[str, list[Path]] = {}
    extra: set[str] = set()

    paths = sorted(
        path for path in libft_dir.rglob("*.c")
        if not relative_is_ignored(path, libft_dir)
    )
    for path in paths:
        text = mask_c_comments_and_literals(read_source(path))
        definitions = set(DEFINITION_PATTERN.findall(text))
        if not definitions and path.stem.startswith("ft_") and path.stem in FUNCTION_PATTERN.findall(text):
            definitions = {path.stem}
        calls = set(FUNCTION_PATTERN.findall(text))
        for function in definitions:
            if function in source_by_function and source_by_function[function] != path:
                duplicates.setdefault(function, [source_by_function[function]]).append(path)
                continue
            source_by_function[function] = path
            calls_by_function[function] = calls - {function}
            if function not in SPEC_BY_NAME:
                extra.add(function)
    return Discovery(source_by_function, calls_by_function, duplicates, extra, paths)


def normalize_names(raw_names: Iterable[str]) -> list[str]:
    normalized: list[str] = []
    for raw in raw_names:
        for item in raw.split(","):
            item = item.strip()
            if not item:
                continue
            name = item if item.startswith("ft_") else f"ft_{item}"
            if name not in normalized:
                normalized.append(name)
    return normalized


def libc_has(symbol: str) -> bool:
    try:
        getattr(ctypes.CDLL(None), symbol)
        return True
    except AttributeError:
        return False


def reference_label(spec: TestSpec) -> str:
    if spec.reference is None:
        return "none (behavioral oracle)"
    return f"{spec.reference} ({'available' if libc_has(spec.reference) else 'unavailable'} on this Linux)"


def print_listing(discovery: Discovery) -> None:
    print(f"{'function':<20} {'source':<10} {'reference':<36} group")
    print("-" * 92)
    for spec in SPECS:
        found = "found" if spec.name in discovery.source_by_function else "missing"
        print(f"{spec.name:<20} {found:<10} {reference_label(spec):<36} {spec.group}")
    if discovery.extra_functions:
        print("\nDiscovered ft_ functions without a built-in test:")
        for name in sorted(discovery.extra_functions):
            print(f"  {name} ({discovery.source_by_function[name]})")
    if discovery.duplicates:
        print("\nDuplicate definitions (compilation is blocked for these names):")
        for name, paths in sorted(discovery.duplicates.items()):
            print(f"  {name}: {', '.join(str(path) for path in paths)}")


def source_closure(selected: list[str], discovery: Discovery) -> list[Path]:
    queue = list(selected)
    seen_functions: set[str] = set()
    source_paths: set[Path] = set()
    while queue:
        function = queue.pop(0)
        if function in seen_functions:
            continue
        seen_functions.add(function)
        path = discovery.source_by_function.get(function)
        if path is None:
            continue
        source_paths.add(path)
        for dependency in sorted(discovery.calls_by_function.get(function, set())):
            if dependency in discovery.source_by_function and dependency not in seen_functions:
                queue.append(dependency)
    return sorted(source_paths)


def find_header(libft_dir: Path) -> Path | None:
    preferred = [libft_dir / "libft.h", libft_dir / "include" / "libft.h"]
    for candidate in preferred:
        if candidate.is_file():
            return candidate
    matches = sorted(
        path for path in libft_dir.rglob("libft.h")
        if not relative_is_ignored(path, libft_dir)
    )
    if len(matches) > 1:
        raise RuntimeError(
            "multiple libft.h files found; place the authoritative header at the "
            f"project root or include/libft.h: {', '.join(str(path) for path in matches)}"
        )
    return matches[0] if matches else None


def macro_for(name: str) -> str:
    return "TEST_" + name.upper()


def generate_translation_unit(path: Path, selected: list[str], runner_path: Path) -> None:
    defines = "\n".join(f"#define {macro_for(name)} 1" for name in selected)
    path.write_text(
        "/* Generated by libft_tester.py; do not edit. */\n"
        f"{defines}\n"
        f'#include "{runner_path.as_posix()}"\n',
        encoding="utf-8",
    )


def shell_join(command: list[str]) -> str:
    return " ".join(shlex.quote(part) for part in command)


def compile_runner(
    args: argparse.Namespace,
    libft_dir: Path,
    header: Path,
    selected: list[str],
    sources: list[Path],
    build_dir: Path,
) -> Path:
    project_dir = Path(__file__).resolve().parent
    runner_path = project_dir / "src" / "runner.c"
    generated = build_dir / "selected_runner.c"
    binary = build_dir / "libft_tests"
    generate_translation_unit(generated, selected, runner_path)

    include_dirs = {header.parent, libft_dir}
    command = [
        *shlex.split(args.cc),
        "-Wall",
        "-Wextra",
        "-Werror",
        "-ffunction-sections",
        "-fdata-sections",
        "-D_DEFAULT_SOURCE",
    ]
    for include_dir in sorted(include_dirs):
        command.extend(["-I", str(include_dir)])
    command.extend(args.cflag)
    command.append(str(generated))
    command.extend(str(source) for source in sources)
    command.extend(["-Wl,--gc-sections", "-ldl", "-o", str(binary)])

    print("\n[compile]", shell_join(command))
    completed = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    if completed.stdout:
        print(completed.stdout, end="")
    if completed.returncode != 0:
        raise RuntimeError(f"compilation failed with exit status {completed.returncode}")
    return binary


def compile_self_check(args: argparse.Namespace, header: Path, build_dir: Path) -> None:
    project_dir = Path(__file__).resolve().parent
    generated = build_dir / "all_tests_syntax.c"
    generate_translation_unit(generated, [spec.name for spec in SPECS], project_dir / "src" / "runner.c")
    command = [
        *shlex.split(args.cc),
        "-Wall",
        "-Wextra",
        "-Werror",
        "-D_DEFAULT_SOURCE",
        "-I",
        str(header.parent),
        *args.cflag,
        "-fsyntax-only",
        str(generated),
    ]
    print("[self-check]", shell_join(command))
    completed = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    if completed.stdout:
        print(completed.stdout, end="")
    if completed.returncode != 0:
        raise RuntimeError(f"test-suite syntax check failed with exit status {completed.returncode}")
    print("[self-check] all 43 test branches compile cleanly")


def normalize_process_output(output: str | bytes | None) -> str:
    if output is None:
        return ""
    if isinstance(output, bytes):
        return output.decode(errors="replace")
    return output


def run_selected(binary: Path, selected: list[str], timeout: float) -> int:
    failed = 0
    for name in selected:
        spec = SPEC_BY_NAME[name]
        print(f"\n{'=' * 72}\n{name}\nLinux reference: {reference_label(spec)}\n{'=' * 72}")
        process = subprocess.Popen(
            [str(binary), name],
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            start_new_session=True,
        )
        try:
            output, _ = process.communicate(timeout=timeout)
        except subprocess.TimeoutExpired as exc:
            os.killpg(process.pid, signal.SIGKILL)
            trailing, _ = process.communicate()
            partial = normalize_process_output(exc.output)
            trailing_text = normalize_process_output(trailing)
            output = trailing_text if trailing_text.startswith(partial) else partial + trailing_text
            if output:
                print(output, end="" if output.endswith("\n") else "\n")
            print(f"[CRASH] {name} exceeded {timeout:g}s timeout")
            failed += 1
            continue
        output = normalize_process_output(output)
        if output:
            print(output, end="" if output.endswith("\n") else "\n")
        if process.returncode < 0:
            print(f"[CRASH] {name} terminated by signal {-process.returncode}")
            failed += 1
        elif process.returncode != 0:
            print(f"[FAIL] {name} exited with status {process.returncode}")
            failed += 1
        else:
            print(f"[PASS] {name} process completed")
    print(f"\nSummary: {len(selected) - failed}/{len(selected)} function processes passed")
    return 0 if failed == 0 else 1


def main() -> int:
    args = parse_args()
    libft_dir = args.libft.expanduser().resolve()
    if not libft_dir.is_dir():
        print(f"error: Libft directory does not exist: {libft_dir}", file=sys.stderr)
        return 2

    discovery = discover(libft_dir)
    if args.list:
        print_listing(discovery)
        if not args.all and not args.functions and not args.self_check:
            return 0

    names = normalize_names(args.functions)
    if args.all:
        names = [spec.name for spec in SPECS if spec.name in discovery.source_by_function]
        missing_mandatory = [
            spec.name for spec in SPECS
            if spec.group != "bonus-list" and spec.name not in discovery.source_by_function
        ]
        if missing_mandatory:
            print(
                "error: --all requires every mandatory subject function; missing: "
                + ", ".join(missing_mandatory),
                file=sys.stderr,
            )
            return 2
    if not names and not args.self_check:
        print("error: choose --list, --all, --self-check, or at least one function", file=sys.stderr)
        return 2

    unsupported = [name for name in names if name not in SPEC_BY_NAME]
    if unsupported:
        print(f"error: no test implementation for: {', '.join(unsupported)}", file=sys.stderr)
        return 2
    missing = [name for name in names if name not in discovery.source_by_function]
    if missing:
        print(f"error: source definition not found for: {', '.join(missing)}", file=sys.stderr)
        return 2
    duplicate = [name for name in names if name in discovery.duplicates]
    if duplicate:
        print(f"error: duplicate definitions found for: {', '.join(duplicate)}", file=sys.stderr)
        return 2

    try:
        header = find_header(libft_dir)
    except RuntimeError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2
    if header is None:
        print(f"error: no libft.h found under {libft_dir}", file=sys.stderr)
        return 2

    build_dir = Path(tempfile.mkdtemp(prefix="libft-tester-"))
    if args.keep_build:
        print(f"[build] preserving {build_dir}")
    try:
        if args.self_check:
            compile_self_check(args, header, build_dir)
            if not names:
                return 0
        if args.all_sources:
            sources = discovery.all_source_paths
        else:
            sources = source_closure(names, discovery)
        if args.verbose:
            print("[sources]")
            for source in sources:
                print(f"  {source}")
        binary = compile_runner(args, libft_dir, header, names, sources, build_dir)
        return run_selected(binary, names, args.timeout)
    except (OSError, RuntimeError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1
    finally:
        if not args.keep_build:
            shutil.rmtree(build_dir, ignore_errors=True)


if __name__ == "__main__":
    raise SystemExit(main())
