#!/bin/bash
shopt -s extglob
source ./backup_restore_lib.sh
validate_backup_params $1 $2 $3
