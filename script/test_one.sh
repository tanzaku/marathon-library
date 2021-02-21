#!/bin/bash

set -eu

MANAGER=/Users/user/work/src/samurai-coding-2019-20/Software-for-IPSJ-International-AI-Programming-Contest-SamurAI-Coding-2019-2020-master/manager/manager

MAP_DIR=/Users/user/work/src/samurai-coding-2019-20/Software-for-IPSJ-International-AI-Programming-Contest-SamurAI-Coding-2019-2020-master/preliminary-fields
# MAP=${MAP_DIR}/cc-60th_anniversary.dighere
# MAP=${MAP_DIR}/kk5-samurai.dighere
# MAP=${MAP_DIR}/cc-cavebare.dighere

# dog test
# MAP=${MAP_DIR}/mm05.dighere

# MAP=${MAP_DIR}/mm06.dighere

# samurai bad
# MAP=${MAP_DIR}/tt-maze2.dighere

# MAP=${MAP_DIR}/tt-rescue2.dighere
# MAP=${MAP_DIR}/yy-separate1.dighere
# MAP=${MAP_DIR}/kk7-explicit.dighere
# MAP=${MAP_DIR}/kk2-savedog.dighere
# MAP=${MAP_DIR}/kk3-zigzag.dighere
# MAP=${MAP_DIR}/kk8-digorgo.dighere
# MAP=${MAP_DIR}/kk6-moat.dighere
# MAP=${MAP_DIR}/cc-release.dighere
MAP=${MAP_DIR}/cc-holeriver.dighere





SAMPLE_PLAYER=/Users/user/work/src/samurai-coding-2019-20/Software-for-IPSJ-International-AI-Programming-Contest-SamurAI-Coding-2019-2020-master/players/simplePlayer

cargo build --release
MY_AI=/Users/user/work/src/samurai-coding-2019-20/tanzaku_ai/target/release/tanzaku_ai

# cargo build
# MY_AI=/Users/user/work/src/samurai-coding-2019-20/tanzaku_ai/target/debug/tanzaku_ai

LOG=/Users/user/work/src/samurai-coding-2019-20/tanzaku_ai/player.dighere

# IOLOG=/Users/user/work/src/samurai-coding-2019-20/tanzaku_ai/log
# ${MANAGER} -D ${IOLOG} ${MAP} ${SAMPLE_PLAYER} ${MY_AI} > ${LOG}
# ${MANAGER} ${MAP} ${SAMPLE_PLAYER} ${MY_AI} > ${LOG}
${MANAGER} ${MAP} ${MY_AI} ${SAMPLE_PLAYER} > ${LOG}

# ${MANAGER} ${MAP} ${MY_AI} ${MY_AI} > ${LOG}

(sudo pkill manager & sudo pkill tanzaku_ai & sudo pkill simplePlayer) && :
