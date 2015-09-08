#! /bin/bash

infile=$1
outfile_base=`echo $infile | sed 's/\./_/g'`
outfile=$outfile_base.h

# Minify first; tool requires .htm input extension
intmpfile=$(tempfile -d. -p min_ -s.htm) || exit
outtmpfile=`basename $intmpfile .htm`.html
outtmpfile_base=`echo $outtmpfile | sed 's/\./_/g'`
trap "rm -f -- '$intmpfile' '$outtimpfile'" EXIT
echo "Temp file: $intmpfile"
cp $infile $intmpfile
css-html-js-minify.py --quiet $intmpfile

echo '/* Auto-generated from '"${infile}"' -- DO NOT EDIT! */' > $outfile
echo "#ifndef ${outfile_base}_inc" >> $outfile
echo "#define ${outfile_base}_inc" >> $outfile
echo "#include <inttypes.h>" >> $outfile

# Explanations:
# - Rename variables (don't use tempfile name)
# - Move array to program flash (save RAM)
# - Use <inttypes.h> names
# - Nul-terminate
# - Remove _len variable, sizeof() works just fine
xxd -i $outtmpfile | \
sed "s/${outtmpfile_base}/${outfile_base}/g" | \
sed 's/\[\] = /\[\] PROGMEM = /g' | \
sed 's/unsigned char/const uint8_t/g' | \
sed 's/0x\([0-9a-f][0-9a-f]\)$/0x\1, 0x00/g' | \
grep -v "${outfile_base}_len" >> $outfile

echo "#endif  // ${outfile_base}_inc" >> $outfile


rm -f "$intmpfile" "$outtmpfile"
trap - EXIT

