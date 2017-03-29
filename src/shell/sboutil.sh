#!/bin/bash

################################################################################
# Gets SlackBuild name from installed package name in /var/log/packages
function get_pkg_name ()
{
  local PKG=$1

  # Get rid of trailing stuff
  local BUILD=${PKG%*-*}  # Build-number
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
  local VERSION=${PKG%*-*}  # Build-number
  VERSION=${VERSION%*-*}  # Architecture

  # Get rid of leading package name
  local TEMP=${VERSION%*-*}
  VERSION=${VERSION#$TEMP-*}
  echo $VERSION
}

################################################################################
# Returns version and package name of an installed SlackBuild. If not installed,
# returns "not_installed" for both. Also checks whether the package originated
# from the expected repository by checking the tag.
function get_installed_info ()
{
  local BUILD=$1
  local PKGLIST=$(find /var/log/packages -maxdepth 1 -name "$BUILD*")
  local VERSION="not_installed"
  local PKGNAME="not_installed"
  local FOREIGN=0
  local PKG BUILDNAME
  local TAGLEN=${#TAG}

  # There can be multiple packages fitting the pattern, so loop through them
  # and check against requested
  if [ -n "$PKGLIST" ]; then
    for PKG in $PKGLIST
    do
      PKG=$(basename "$PKG")
      BUILDNAME=$(get_pkg_name "$PKG")
      if [ "$BUILDNAME" == "$BUILD" ]; then
        VERSION=$(get_pkg_version "$PKG")
        PKGNAME=$PKG
        if [ "${PKG:(-$TAGLEN)}" != "$TAG" ]; then
          FOREIGN=1
        fi 
        break
      fi
    done
  fi

  echo $VERSION
  echo $PKGNAME
  echo $FOREIGN
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
  local PKGLIST=$(find /var/log/packages -maxdepth 1 -name "$BUILD*")
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

  if [ -n "$REQUIRES" ]; then
    echo $REQUIRES
  fi
}

case $1 in
    "get_installed_info")
        get_installed_info $2
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
