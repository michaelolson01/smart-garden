#!/bin/bash
# scripts/gen-changelog.sh

latest_tag=$(git describe --tags --abbrev=0)
echo "## [Unreleased] - $(date +%Y-%m-%d)"
echo

git log "$latest_tag"..HEAD --pretty=format:"- %s" | sed 's/^/- /'
