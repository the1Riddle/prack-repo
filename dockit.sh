#!/usr/bin/env bash

set -e

USER_NAME="student"
YOURC="/home/$USER/.bashrc"

if [ -n "$ZSH_VERSION" ] || [[ "$SHELL" == *"zsh"* ]]; then
    BASHRC="/home/$USER_NAME/.zshrc"
    YOURC="/home/$USER/.zshrc"
elif [ -n "$BASH_VERSION" ] || [[ "$SHELL" == *"bash"* ]]; then
    BASHRC="/home/$USER_NAME/.bashrc"
else
    # Fallback default
    BASHRC="$HOME/.profile"
fi

print_help() {
cat <<EOF

Rootless Docker setup completed.

Next steps:

1. Reload your shell:
   source ~/.bashrc

2. Start the Docker daemon:
   docksta

3. Open a new terminal tab/window.

4. Switch to the docker environment:
   dockenv

5. Verify Docker is working:
   docker ps

You can now run Docker commands without sudo.

Useful aliases:
  dockenv  -> switch to docker environment
  docksta  -> start rootless Docker daemon

EOF
}

# Handle help flag
if [[ "$1" == "--help" || "$1" == "-h" ]]; then
    print_help
    exit 0
fi

echo "[+] Configuring aliases and environment variables..."


grep -qxF "alias dockenv='su - $USER_NAME'" "$YOURC" || echo "alias dockenv='su - $USER_NAME'" >> "$YOURC"

echo "dockenv.......Done!"

su - "$USER_NAME" -c "grep -qxF 'export XDG_RUNTIME_DIR=/home/$USER_NAME/.docker/run' '$BASHRC' || echo 'export XDG_RUNTIME_DIR=/home/$USER_NAME/.docker/run' >> '$BASHRC'"
su - "$USER_NAME" -c "grep -qxF 'export PATH=/usr/local/bin:\$PATH' '$BASHRC' || echo 'export PATH=/usr/local/bin:\$PATH' >> '$BASHRC'"
su - "$USER_NAME" -c "grep -qxF 'export DOCKER_HOST=unix:///home/$USER_NAME/.docker/run/docker.sock' '$BASHRC' || echo 'export DOCKER_HOST=unix:///home/$USER_NAME/.docker/run/docker.sock' >> '$BASHRC'"

su - "$USER_NAME" << EOF
grep -qxF "alias docksta='PATH=/usr/local/bin:/sbin:/usr/sbin:\$PATH dockerd-rootless.sh --exec-opt native.cgroupdriver=cgroupfs'" "$BASHRC" || \
echo "alias docksta='PATH=/usr/local/bin:/sbin:/usr/sbin:\$PATH dockerd-rootless.sh --exec-opt native.cgroupdriver=cgroupfs'" >> "$BASHRC"
EOF

echo "[+] Installing rootless Docker for $USER..."

su - "$USER_NAME" -c "dockerd-rootless-setuptool.sh install"

echo
echo "[✓] Setup complete."

print_help
