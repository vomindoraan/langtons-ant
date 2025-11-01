#!/usr/bin/env bash
SCRIPTS_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"
BRANCH=${1:-develop}

cd "$SCRIPTS_DIR"
echo "Updating from '$BRANCH'..."
set -e
git checkout $BRANCH; git fetch; git reset --hard origin/$BRANCH
./build.sh
