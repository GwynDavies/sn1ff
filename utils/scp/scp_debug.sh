#!/bin/bash

# MIT License
#
# Copyright (c) 2025 Gwyn Davies
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR,
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

USER="sn1ff"
CHROOT_BASE="/home/chroot"
JAIL_ROOT="$CHROOT_BASE/$USER"
UPLOAD_DIR="$JAIL_ROOT/upload"

echo "🔍 Checking chroot jail for user: $USER"
echo "Chroot base: $CHROOT_BASE"
echo "Jail root: $JAIL_ROOT"
echo

# 1. Check if user exists
if id "$USER" >/dev/null 2>&1; then
    echo "✅ User $USER exists."
else
    echo "❌ User $USER does not exist."
    exit 1
fi

# 2. Check shell
USER_SHELL=$(getent passwd "$USER" | cut -d: -f7)
echo "🔧 Shell: $USER_SHELL"

if [[ "$USER_SHELL" != "/bin/bash" ]]; then
    echo "⚠️ Warning: shell is not /bin/bash (current: $USER_SHELL)"
fi

# 3. Check chroot path
if [[ ! -d "$JAIL_ROOT" ]]; then
    echo "Chroot path $JAIL_ROOT does not exist."
    exit 1
else
    echo "✅ Chroot path exists."
fi

# 4. Check upload directory inside jail
echo
echo "📁 Checking $UPLOAD_DIR..."

if [[ -d "$UPLOAD_DIR" ]]; then
    ls -ld "$UPLOAD_DIR"
    
    OWNER=$(stat -c "%U" "$UPLOAD_DIR")
    GROUP=$(stat -c "%G" "$UPLOAD_DIR")
    PERMS=$(stat -c "%A" "$UPLOAD_DIR")

    echo "👤 Owner: $OWNER"
    echo "👥 Group: $GROUP"
    echo "🔐 Permissions: $PERMS"

    if [[ "$OWNER" == "$USER" && "$PERMS" == d*r*wx* ]]; then
        echo "✅ Directory appears writable by $USER"
    else
        echo "⚠️ May not be writable by $USER. Consider running:"
        echo "    sudo chown $USER:$USER $UPLOAD_DIR"
        echo "    sudo chmod 755 $UPLOAD_DIR"
    fi
else
    echo "❌ $UPLOAD_DIR does not exist."
    echo "👉 Run:"
    echo "    sudo mkdir -p $UPLOAD_DIR"
    echo "    sudo chown $USER:$USER $UPLOAD_DIR"
    echo "    sudo chmod 755 $UPLOAD_DIR"
fi

echo
echo "✅ Done."

