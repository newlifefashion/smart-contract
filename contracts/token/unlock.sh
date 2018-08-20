#!/usr/bin/env bash
set -x
set -e

cleos ${remote} push action ${contract} unlock '[bob111111111]' -p bob111111111