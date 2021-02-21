#!/bin/bash

set -eu

cargo build --release

BASE_DIR=/Users/user/work/src/samurai-coding-2019-20/Software-for-IPSJ-International-AI-Programming-Contest-SamurAI-Coding-2019-2020-master
MANAGER=${BASE_DIR}/manager/manager
MAP_DIR=${BASE_DIR}/preliminary-fields
MAP=${MAP_DIR}/cc-60th_anniversary.dighere
PRELIMINARILY_PLAYER=${BASE_DIR}/players/simplePlayer
MY_AI=/Users/user/work/src/samurai-coding-2019-20/tanzaku_ai/target/release/tanzaku_ai

for file in $(find ${MAP_DIR} -maxdepth 1 -type f); do
    LOG=/Users/user/work/src/samurai-coding-2019-20/tanzaku_ai/logs/$(basename ${file})
    ${MANAGER} ${MAP} ${PRELIMINARILY_PLAYER} ${MY_AI} > ${LOG}
    # cat ${file}
    # echo ${LOG}
    # sudo pkill manager
    # sudo pkill tanzaku_ai
    # sudo pkill simplePlayer
done


