#!/usr/bin/env bash
BRANCH=develop
SCRIPTS_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"

git checkout $BRANCH && git fetch && git reset --hard origin/$BRANCH
"$SCRIPTS_DIR"/build.sh
