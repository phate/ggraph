#!/usr/bin/env bash

dotf=$(basename $1)
psf="${dotf%.*}".ps
pdff="${psf%.*}".pdf

dot -Tps $dotf > $psf
echo Wrote $psf
ps2pdf $psf
echo Wrote $pdff

