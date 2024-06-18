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
track.

```
root -l -b -q load.C+ runEventLoop.C+
```

## The major offerings of the MAT
At minimum, MAT's ChainWrapper can immediately replace ROOTs MakeClass for
looping entries and accessing branches: it's faster and has an easier
branch access interface:

```
chain->GetDouble("my_branch");
chain->GetVectorInt("my_int_vector_branch");
```

The MAT and its histogram objects also use a "many universe" approach to
systematics. For a single event, a histogram's bin counts are calculated in all
user-defined universes, subject to different cuts or event weights. The
resulting systematics and histogram algebra/manipulation are handled and
propagated. `CVUniverse.h` defines how variables are calculated by default.
In `Systematics.h`, user defines how those variables should behave differently.

## Installation on mu2e gpvms
```
mu2einit
cd /your/working/directory
muse backing HEAD
```
