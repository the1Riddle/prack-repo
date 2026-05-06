#!/usr/bin/env bash

set -euo pipefail

mkdir -p $HOME/.go


if ! grep -q 'export GOPATH="$HOME/.go"' "$HOME/.bashrc"; then
cat << 'EOF' >> "$HOME/.bashrc"

# Go environment
export GOPATH="$HOME/.go"
export GOMODCACHE="$HOME/.cache/go-mod"
export GOCACHE="$HOME/.cache/go-build"

if [[ ":$PATH:" != *":$HOME/.go/bin:"* ]]; then
  export PATH="$PATH:$HOME/.go/bin"
fi
EOF
fi

source ~/.bashrc

echo "Done!"
