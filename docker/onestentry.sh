#!/bin/bash
ONESTD="/usr/local/bin/witness_node"

# For blockchain download
VERSION=`cat /etc/onest/version`

## Supported Environmental Variables
#
#   * $ONESTD_SEED_NODES
#   * $ONESTD_RPC_ENDPOINT
#   * $ONESTD_PLUGINS
#   * $ONESTD_REPLAY
#   * $ONESTD_RESYNC
#   * $ONESTD_P2P_ENDPOINT
#   * $ONESTD_WITNESS_ID
#   * $ONESTD_PRIVATE_KEY
#   * $ONESTD_TRACK_ACCOUNTS
#   * $ONESTD_PARTIAL_OPERATIONS
#   * $ONESTD_MAX_OPS_PER_ACCOUNT
#   * $ONESTD_ES_NODE_URL
#   * $ONESTD_ES_START_AFTER_BLOCK
#   * $ONESTD_TRUSTED_NODE
#

ARGS=""
# Translate environmental variables
if [[ ! -z "$ONESTD_SEED_NODES" ]]; then
    for NODE in $ONESTD_SEED_NODES ; do
        ARGS+=" --seed-node=$NODE"
    done
fi
if [[ ! -z "$ONESTD_RPC_ENDPOINT" ]]; then
    ARGS+=" --rpc-endpoint=${ONESTD_RPC_ENDPOINT}"
fi

if [[ ! -z "$ONESTD_REPLAY" ]]; then
    ARGS+=" --replay-blockchain"
fi

if [[ ! -z "$ONESTD_RESYNC" ]]; then
    ARGS+=" --resync-blockchain"
fi

if [[ ! -z "$ONESTD_P2P_ENDPOINT" ]]; then
    ARGS+=" --p2p-endpoint=${ONESTD_P2P_ENDPOINT}"
fi

if [[ ! -z "$ONESTD_WITNESS_ID" ]]; then
    ARGS+=" --witness-id=$ONESTD_WITNESS_ID"
fi

if [[ ! -z "$ONESTD_PRIVATE_KEY" ]]; then
    ARGS+=" --private-key=$ONESTD_PRIVATE_KEY"
fi

if [[ ! -z "$ONESTD_TRACK_ACCOUNTS" ]]; then
    for ACCOUNT in $ONESTD_TRACK_ACCOUNTS ; do
        ARGS+=" --track-account=$ACCOUNT"
    done
fi

if [[ ! -z "$ONESTD_PARTIAL_OPERATIONS" ]]; then
    ARGS+=" --partial-operations=${ONESTD_PARTIAL_OPERATIONS}"
fi

if [[ ! -z "$ONESTD_MAX_OPS_PER_ACCOUNT" ]]; then
    ARGS+=" --max-ops-per-account=${ONESTD_MAX_OPS_PER_ACCOUNT}"
fi

if [[ ! -z "$ONESTD_ES_NODE_URL" ]]; then
    ARGS+=" --elasticsearch-node-url=${ONESTD_ES_NODE_URL}"
fi

if [[ ! -z "$ONESTD_ES_START_AFTER_BLOCK" ]]; then
    ARGS+=" --elasticsearch-start-es-after-block=${ONESTD_ES_START_AFTER_BLOCK}"
fi

if [[ ! -z "$ONESTD_TRUSTED_NODE" ]]; then
    ARGS+=" --trusted-node=${ONESTD_TRUSTED_NODE}"
fi

## Link the ONEST config file into home
## This link has been created in Dockerfile, already
ln -f -s /etc/ONEST/config.ini /var/lib/ONEST

# Plugins need to be provided in a space-separated list, which
# makes it necessary to write it like this
if [[ ! -z "$ONESTD_PLUGINS" ]]; then
   exec $ONESTD --data-dir ${HOME} ${ARGS} ${ONESTD_ARGS} --plugins "${ONESTD_PLUGINS}"
else
   exec $ONESTD --data-dir ${HOME} ${ARGS} ${ONESTD_ARGS}
fi
