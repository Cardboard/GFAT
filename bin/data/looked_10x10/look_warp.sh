#!/bin/bash

datafld=/home/bminchew/nobak/Antarctica/analysis/cosmo/Rutford/tracks/fourDvel_prelim/new_losd_reg7
look=10
tsrs="EPSG:32716"

cp $datafld/fourDvel.hdr .
hdr=fourDvel.hdr
cols=`grep 'samples =' $hdr | awk '{print $NF}'`
null=`grep 'output null value' $hdr | awk '{print $NF}'`
echo Data Ignore Value = $null >> $hdr

for m in `ls $datafld/fourDvel.*.flt.msk | grep -v 'fourDvel.*.flt.flt.msk'`; do 
    echo $m
    f=`basename $m`.look
    echo $f
    look_r4_bm $m $cols $look $look $f $null
    cp $hdr $f\.hdr
    ./fixhdr4looks.py $f\.hdr $look

    echo $f
    gdalwarp $f $f\.utm -t_srs $tsrs -of ENVI
    sarmath.py $f\.utm $null -null2nan = $f\.utm
done
