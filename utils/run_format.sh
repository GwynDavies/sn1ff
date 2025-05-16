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

cd ..

# --------------------------------------------------#
#                                                   #
# Format c files                                    #
#                                                   #
# --------------------------------------------------#

# sudo apt install clang-format

find . -name '*.[ch]' -exec clang-format -i {} +

# --------------------------------------------------#
#                                                   #
# Format bash script files                          #
#                                                   #
# --------------------------------------------------#

# sudo apt install shfmt

# Indent all .sh files in the "examples" directory
for file in examples/*.sh; do
  shfmt -i 2 -ci -w -bn "$file"
done

# Indent all .sh files in the "examples/lib" directory
for file in examples/lib/*.sh; do
  shfmt -i 2 -ci -w -bn "$file"
done

# Indent all sh files in the "debian" directory
shfmt -i 2 -ci -w -bn debian/prerm-server
shfmt -i 2 -ci -w -bn debian/postinst-server  
shfmt -i 2 -ci -w -bn debian/postrm-server

