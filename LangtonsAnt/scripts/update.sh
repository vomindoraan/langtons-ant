#!/usr/bin/env bash
BRANCH=develop
SCRIPTS_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"

set -e
cd "$SCRIPTS_DIR"
git checkout $BRANCH && git fetch && git reset --hard origin/$BRANCH
./build.sh
