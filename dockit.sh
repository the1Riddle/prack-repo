#!/usr/bin/env bash

set -e

# This script is not fully compatible with (zsh)'s
# It will break when your terminal uses bashrc and your docker env uses zsh or the other way round
# the work arround is to find where line 65 onwords to 75 was writen on and update the apropriate rc file.
# after, reload the rc file and everything should work as intended.
# by: whoever wrote the script :P.

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

print_error() {
cat <<EOF

Rootless Docker setup Error.

If you get an error while executing docksta command:

1. Reload your shell and move to your docker environment:
   source ~/.bashrc
   dockenv

2. If the docker env shel is a zsh, run the command:
    cat ~/.bashrc and coppy the lines from the statement:
    ====> # Docker Script to your rc file START
    to the line:
    ====> # Docker Script to your rc file END
    then add it to your ~/.zshrc

3. If you could not find the lines open ~/.zshrc and find the lins:
    if you found them on ~/.zshrc add the lines to ~/.bashrc

4. Reload your dockenv terminal depending on which shell it is:
    run either =====> source ~/.bashrc
        or     =====> source ~/.zshrc

5. Verify docksta command works:
    docksta

if it runs you are good to go.

EOF
}

# Handle help flag
if [[ "$1" == "--help" || "$1" == "-h" ]]; then
    print_help
    exit 0
fi

if [[ "$1" == "--error" || "$1" == "-e" || "$1" == "wtf" ]]; then
    print_error
    exit 0
fi

echo "[+] Configuring aliases and environment variables..."


grep -qxF "alias dockenv='su - $USER_NAME'" "$YOURC" || echo "alias dockenv='su - $USER_NAME'" >> "$YOURC"

echo "dockenv.......Done!"

su - "$USER_NAME" -c "grep -qxF '# Docker Script to your rc file START' '$BASHRC' || echo '# Docker Script to your rc file START' >> '$BASHRC'"
su - "$USER_NAME" -c "grep -qxF 'export XDG_RUNTIME_DIR=/home/$USER_NAME/.docker/run' '$BASHRC' || echo 'export XDG_RUNTIME_DIR=/home/$USER_NAME/.docker/run' >> '$BASHRC'"
su - "$USER_NAME" -c "grep -qxF 'export PATH=/usr/local/bin:\$PATH' '$BASHRC' || echo 'export PATH=/usr/local/bin:\$PATH' >> '$BASHRC'"
su - "$USER_NAME" -c "grep -qxF 'export DOCKER_HOST=unix:///home/$USER_NAME/.docker/run/docker.sock' '$BASHRC' || echo 'export DOCKER_HOST=unix:///home/$USER_NAME/.docker/run/docker.sock' >> '$BASHRC'"
su - "$USER_NAME" -c "grep -qxF '# Docker Script to your rc file END' || echo '# Docker Script to your rc file END' >> '$BASHRC'"

su - "$USER_NAME" << EOF
grep -qxF "alias docksta='PATH=/usr/local/bin:/sbin:/usr/sbin:\$PATH dockerd-rootless.sh --exec-opt native.cgroupdriver=cgroupfs'" "$BASHRC" || \
echo "alias docksta='PATH=/usr/local/bin:/sbin:/usr/sbin:\$PATH dockerd-rootless.sh --exec-opt native.cgroupdriver=cgroupfs'" >> "$BASHRC"
EOF

echo "[+] Installing rootless Docker for $USER..."

su - "$USER_NAME" -c "dockerd-rootless-setuptool.sh install"

echo
echo "[✓] Setup complete."

print_help
