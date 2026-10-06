#!/usr/bin/env bash
set -euo pipefail

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [[ $# -eq 0 ]]; then
    echo "No Libft path supplied; running the bundled validation fixture."
    exec python3 "$PROJECT_DIR/libft_tester.py" \
        --libft "$PROJECT_DIR/fixtures/mini_libft" \
        strlen memmove strlcpy substr split putchar_fd lstnew lstsize lstclear
fi

exec python3 "$PROJECT_DIR/libft_tester.py" "$@"
