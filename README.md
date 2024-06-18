# MAT Barebones Test for Mu2e

## Introduction
The MINERvA Analysis Toolkit ([MAT](https://github.com/MinervaExpt/MAT)) is a
toolkit for at looping root tuples, filling histograms, and
calculating/propagating systematics. It's the foundation for dozens of MINERvA
publications and counting.

Paper here: [Arxiv
 2103.08677](https://arxiv.org/abs/2103.08677).

In this repo I show here how it can work on Mu2e. `runEventLoop.C` loops over
TrkAna tree entries, makes cuts, and plots a branch: the number of hits in a
track. Run it with (and see instructions below):

```
root -l -b -q load.C+ runEventLoop.C+
```

## The major offerings of the MAT
**Replace MakeClass:** At minimum, MAT's ChainWrapper can immediately replace
ROOTs MakeClass for looping entries and accessing branches: it's faster and has
an easier branch access interface:

```
chain->GetDouble("my_branch");
chain->GetVectorInt("my_int_vector_branch");
```
No messy header loading all branches. Each branch is loaded as needed and is
cached for subsequent loops.

**Systematics** The MAT makes it easy to start doing systematics analysis
immediately. It uses "many universe" approach to systematics: For a single
event, a histogram's bin counts are re-calculated in each user-defined
universe, subject to cuts or event weights. Resulting systematics and
histogram algebra/manipulation are handled and propagated. `CVUniverse.h`
defines how variables are calculated in the central value. And in
`Systematics.h`, user defines how those variables should behave differently in
each universe.

## Installation on mu2e gpvms

```
# standard mu2e setup
cd /your/working/directory
mu2einit
setup mu2e
muse setup TrkAna v05_00_00

# the MAT
git clone https://github.com/MinervaExpt/MAT.git
mkdir -p opt/build && cd opt/build
cmake ../../MAT -DCMAKE_INSTALL_PREFIX=`pwd`/.. -DCMAKE_BUILD_TYPE=Release
make install #-j 4
source setup_MAT.sh 
cd ../..

# get some tutorial trkana files
mkdir filelists
setup dhtools
samListLocations -d --defname=nts.mu2e.CeEndpointMix1BBSignal.Tutorial_2024_03.tka > filelists/nts.mu2e.CeEndpointMix1BBSignal.Tutorial_2024_03.list

# download this repo and run the loop
git clone https://github.com/bamesserly/mu2e-mat.git
cd mu2e-mat/
root -l -q -b load.C+ runEventLoop.C+
```
