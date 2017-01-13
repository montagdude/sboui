#!/bin/bash

################################################################################
# Removes quotes around string
function remove_quotes ()
{
  local STRING=$1
  STRING=${STRING#\"}
  STRING=${STRING%\"}
  echo $STRING
}

################################################################################
# Gets SlackBuild name from installed package name in /var/log/packages
function get_pkg_name ()
{
  local PKG=$1

  # Get rid of trailing stuff
  local BUILD=${PKG%*_$TAG}
  BUILD=${BUILD%*-*}  # Architecture-number
  BUILD=${BUILD%*-*}  # Architecture
  BUILD=${BUILD%*-*}  # Version
  echo $BUILD
}

################################################################################
# Gets SlackBuild version from installed package name in /var/log/packages
function get_pkg_version ()
{
  local PKG=$1

  # Get rid of trailing stuff
  local VERSION=${PKG%*_$TAG}
  VERSION=${VERSION%*-*}  # Architecture-number
  VERSION=${VERSION%*-*}  # Architecture

  # Get rid of leading package name
  local TEMP=${VERSION%*-*}
  VERSION=${VERSION#$TEMP-*}
  echo $VERSION
}

################################################################################
# Checks if a SlackBuild is actually installed. Returns installed version if so;
# returns "not_installed" otherwise.
function check_installed ()
{
  local BUILD=$1
  local PKGLIST=$(find /var/log/packages -maxdepth 1 -name "$BUILD*_$TAG")
  #local PKGLIST=$(find /data/dprosser/software/sboui_files/packages -maxdepth 1 -name "$BUILD*_$TAG")
  local INSTALLED="not_installed"
  local PKG BUILDNAME

  # There can be multiple packages fitting the pattern, so loop through them
  # and check against requested
  if [ -n "$PKGLIST" ]; then
    for PKG in $PKGLIST
    do
      PKG=$(basename "$PKG")
      BUILDNAME=$(get_pkg_name "$PKG")
      if [ "$BUILDNAME" == "$BUILD" ]; then
        INSTALLED=$(get_pkg_version "$PKG")
        break
      fi
    done
  fi

  echo $INSTALLED
}

################################################################################
# Gets SlackBuild version from string in .info file
function get_available_version ()
{
  local BUILD=$1
  local CATEGORY=$2

  # Read .info file
  . $REPO_DIR/$CATEGORY/$BUILD/$BUILD.info

  echo $VERSION
}

################################################################################
# Lists installed SlackBuilds
function list_installed ()
{
  local PKGLIST=$(find /var/log/packages -maxdepth 1 -name "$BUILD*_$TAG")
  #local PKGLIST=$(find /data/dprosser/software/sboui_files/packages -maxdepth 1 -name "$BUILD*_$TAG")
  local PKG

  for PKG in $PKGLIST
  do
    PKG=$(basename "$PKG")
    echo $(get_pkg_name "$PKG")
  done
}

################################################################################
# Gets SlackBuild dependencies from string in .info file
function get_reqs ()
{
  local BUILD=$1
  local CATEGORY=$2

  # Read .info file
  . $REPO_DIR/$CATEGORY/$BUILD/$BUILD.info

  echo $REQUIRES
}

case $1 in
    "check_installed")
        check_installed $2
        ;;
    "get_available_version")
        get_available_version $2 $3
        ;;
    "list_installed")
        list_installed
        ;;
    "get_reqs")
        get_reqs $2 $3
        ;;
    *)
        ;;
esac
