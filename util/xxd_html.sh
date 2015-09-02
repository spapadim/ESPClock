#! /bin/bash

infile=$1
outfile_base=`echo $infile | sed 's/\./_/g'`
outfile=$outfile_base.h

echo '/* Auto-generated from '"${infile}"' -- DO NOT EDIT! */' > $outfile
echo "#ifndef ${outfile_base}_inc" >> $outfile
echo "#define ${outfile_base}_inc" >> $outfile
echo "#include <inttypes.h>" >> $outfile

# Explanations:
# - Move array to program flash (save RAM)
# - Use <inttypes.h> names
# - Nul-terminate
# - Remove _len variable, sizeof() works just fine
xxd -i $1 | \
sed 's/\[\] = /\[\] PROGMEM = /g' | \
sed 's/unsigned char/const uint8_t/g' | \
sed 's/0x\([0-9a-f][0-9a-f]\)$/0x\1, 0x00/g' | \
grep -v "${outfile_base}_len" >> $outfile

echo "#endif  // ${outfile_base}_inc" >> $outfile
