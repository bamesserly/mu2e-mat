//==============================================================================
// Loop entries, make cuts, fill histograms.
// * Uses the New Systematics Framework and "Universe" objects.
// * loop universes, make cuts and fill histograms with the correct lateral
// shifts and weights for each universe.
// * TChain --> PlotUtils::ChainWrapper.
// * MnvHXD --> PlotUtils::HistWrapper.
// * Genie, flux, non-resonant pion, and some detector systematics calculated.
//==============================================================================
#include <iostream>

#include "CVUniverse.h"
#include "Systematics.h"
#include "PlotUtils/HistWrapper.h"
#include "PlotUtils/makeChainWrapper.h"
#include "TH1.h"
#include "plotting_functions.h"

bool PassesCuts(CVUniverse& univ) {
  return univ.GetBool("demmc.valid") && 
    5 < univ.get_n_track_hits() && univ.get_n_track_hits() < 100;
}
//======================================================================

// Get container of systematics
std::map<std::string, std::vector<CVUniverse*> > GetErrorBands(
    PlotUtils::ChainWrapper* chain) 
{
  typedef std::map<std::string, std::vector<CVUniverse*> > SystMap;

  SystMap error_bands;

  // CV
  error_bands[std::string("CV")].push_back(new CVUniverse(chain));

  error_bands[std::string("ModelX")].push_back(new ModelXUniverse(chain, -1.));
  error_bands[std::string("ModelX")].push_back(new ModelXUniverse(chain, 1.));

  error_bands[std::string("HitCal")].push_back(new TrackHitCalibrationUniverse(chain, -1.));
  error_bands[std::string("HitCal")].push_back(new TrackHitCalibrationUniverse(chain, 1.));

  return error_bands;
}
//======================================================================

// Main
void runEventLoop() {
  TH1::AddDirectory(kFALSE);  // Needed so that MnvH1D gets to clean up its own)

  // Make a chain of events
  PlotUtils::ChainWrapper* chain =
      makeChainWrapperPtr("../filelists/nts.mu2e.CeEndpointMix1BBSignal.Tutorial_2024_03.list", "TrkAna/trkana");

  // Make a map of systematic universes
  std::map<std::string, std::vector<CVUniverse*> > error_bands =
      GetErrorBands(chain);

  // Use the vector of systematic universes to make your MnvH1D
  // (Binning defined in plotting_functions.h)
  PlotUtils::HistWrapper<CVUniverse> h_track_nhits("h_track_nhits", "nhits in track",
                                            nbins, xmin, xmax, error_bands); 

  //=========================================
  // Entry Loop
  //=========================================
  for(int i=0; i<chain->GetEntries(); ++i) {
  //for (int i = 0; i < 25; ++i) {
    if (i % 50000 == 0) std::cout << (i / 1000) << "k " << std::endl;

    //=========================================
    // For every systematic, loop over the universes, and fill the
    // appropriate histogram in the MnvH1D
    //=========================================
    for (auto band : error_bands) {
      std::vector<CVUniverse*> error_band_universes = band.second;
      for (auto universe : error_band_universes) {
        // Tell the Event which entry in the TChain it's looking at
        universe->SetEntry(i);
        //=========================================
        // CUTS in each universe
        //=========================================
        if (PassesCuts(*universe)) {
          int n_hits = universe->get_n_track_hits();
          double wgt = universe->GetWeight();
          h_track_nhits.FillUniverse(*universe, n_hits, wgt);
        }  // End if passed cuts
      }    // End band's universe loop
    }      // End Band loop
  }        // End entries loop

  // This function copies the MnvH1D's CV histo to each error band's CV histos.
  h_track_nhits.SyncCVHistos();

  //=========================================
  // Plot stuff
  //=========================================

  // DrawMCWithErrorBand
  PlotCVAndError(h_track_nhits.hist, "n_track_hits");

  // Plot Error Summary
  PlotErrorSummary(h_track_nhits.hist, "n_track_hits");

  //// Plot individual universes
  //unsigned int universe = 0;
  //PlotVertUniverse("EmuRangeCurve", universe, "TEST", hw_enu.hist);
  //universe = 1;
  //PlotVertUniverse("EmuRangeCurve", universe, "TEST", hw_enu.hist);

  //for (int i = 0; i < 5; ++i) PlotVertUniverse("Flux", i, "TEST", hw_enu.hist);

  //// Plot the Bands
  //PlotVertBand("EmuRangeCurve", "TEST", hw_enu.hist);
  //PlotVertBand("Flux", "TEST", hw_enu.hist);

  PlotTotalError(h_track_nhits.hist, "n_track_hits");

  //=========================================

  //// We have to delete the events ourselves. Should use some sort of
  //// smart pointer to fix that
  // for(auto band : error_bands){
  //  std::vector<CVUniverse*> band_universes = band.second;
  //  for(unsigned int i_universe = 0; i_universe < band_universes.size();
  //  ++i_universe)
  //    delete band_universes[i_universe];
  //}

  std::cout << "Success" << std::endl;
}
