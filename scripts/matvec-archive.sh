#!/bin/sh
#
# matvec tar archive: script must be run in the parent directory of matvec
# ################## 

HOME=.
cd $HOME

LIBVER=$(awk '/GNU Gama.*matvec/ {print $8 ; exit }' matvec/memrep.h | tr --delete /\)/ )
NAMEVER=matvec-$LIBVER
TAR=$NAMEVER.tar.gz


rm -f   $NAMEVER 2>/dev/null
if [ -d $NAMEVER ]; then
   echo
   echo $NAMEVER directory exists - building of tar archive failed!
   echo
   echo
   exit 1
fi
ln -s matvec $NAMEVER

rm -f *.md5sum
MD5=matvec-$LIBVER.md5sum
for s in $(find $NAMEVER/* | egrep \.'(h|cpp|gkf)'$)
do
    md5sum $s >> $MD5
done


rm -f $TAR
echo
echo tar czvf $TAR
echo 
tar czvf $TAR $NAMEVER/* $MD5 --exclude=CVS --exclude=*.o --exclude=*~
echo


if ! test -d archive;
then
   mkdir archive
fi
if ! test -d archive/matvec;
then
   mkdir archive/matvec
fi

cp $TAR archive/matvec
rm $TAR $MD5 $NAMEVER






