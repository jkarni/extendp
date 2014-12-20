#!/bin/bash -
#===============================================================================
#
#          FILE: file-browser.sh
#
#         USAGE: ./file-browser.sh
#
#   DESCRIPTION: A very simple file browser.
#
#  REQUIREMENTS: xp
#        AUTHOR: Julian K. Arni. Idea suggested by Ciprian Craciun.
#       CREATED: 19.12.2014 09:25
#===============================================================================

set -o nounset
set -e

MY_PATH=${0}

CUR_DIR=${1:-`pwd`}

function do_file {
   CUR_DIR=$(readlink -f ${CUR_DIR%%/}/$1)
   if [[ -d "$CUR_DIR" ]] ; then
       echo $CUR_DIR
       OUT=$(ls $CUR_DIR -1 -a | xp -m)
       do_file $OUT
   elif [[ -f $CUR_DIR ]] ; then
       vim $CUR_DIR
   fi
}

do_file ""
