#! /bin/bash

qtBinDir=$1
translationDir=`dirname "$0"`
slateDir=$translationDir/..

if [ ! -d "$qtBinDir" ]; then
    echo "Qt bin directory \"$sourceDir\" doesn't exist; aborting"
    echo
    echo "Usage example:"
    echo
    echo "    lupdate.sh ~/dev/qt5-slate-fw/qtbase/bin"
    exit
fi

set -e

# TODO: find a way to automatically find/list .cpp files

# The first arguments are the input (source code) files.
# The arguments after -ts are the output (.ts) files.
$qtBinDir/lupdate \
    $slateDir/app/qml/qml.qrc $slateDir/lib/filevalidator.cpp \
    -ts $translationDir/slate_en_GB.ts \
    $translationDir/slate_no_NB.ts \
	$translationDir/slate_ar_EG.ts

