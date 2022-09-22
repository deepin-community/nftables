#!/bin/sh

# Run the internal tests of nftables (shell)

# The testsuite requires kernel at least 5.x
if [ "$(uname -r | cut -d. -f1)" -lt 5 ] ; then
	echo "W: this testsuite is likely to produce many fails because of old kernel, ending now"
	exit 0
fi

set -e
cd tests/shell
NFT=$(which nft) ./run-tests.sh -v
