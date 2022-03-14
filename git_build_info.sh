#!/bin/bash -p

GIT="git"
if [ -f "/usr/fsl/bin/git" ]
then
  GIT=/usr/fsl/bin/git
fi

# define a value here to avoid GIT invocation, for platforms like Solaris, RH4
# Example:
# GIT_SHA="72157c9"

if [ -f ".git_sha_static" ]
then
   GIT_SHA=$(cat .git_sha_static)
fi

SHORT_HASH=""
BRANCH=""

if [ "$GIT_HASH" = "" ]
then
    SHORT_HASH=$($GIT rev-parse --short HEAD)
    BRANCH=$($GIT rev-parse --abbrev-ref HEAD)
fi

if [ "$SHORT_HASH" != "" ] && [ "$BRANCH" != "" ]
then
    if [ "$BRANCH" == "master" ]
    then
      GIT_SHA="${SHORT_HASH}"
    else
      GIT_SHA="${BRANCH}:${SHORT_HASH}"
    fi
    echo "$GIT_HASH" > .git_sha
else
  if [ "$GIT_HASH" = "" ]
  then
    # use latest, good for Solaris
    GIT_SHA=$(cat .git_sha)
  fi
fi

export GIT_SHA
echo "$GIT_SHA"
