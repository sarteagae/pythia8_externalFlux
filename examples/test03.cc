// Minimal PYTHIA 8 test of the VMD component in Pb-emitted-photon.
//
// Physics tested:
//   Pb -> Pb + gamma,
//   gamma -> V = rho0, omega, phi, or J/psi,
//   V + p -> V + p   (SoftQCD elastic)
//



#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

#include <math.h>
#include <cmath>

#include "Pythia8/Pythia.h"

using namespace Pythia8;

using namespace std;


/// photon-flux definitioin

class Nucleus2gamma2 : public PDF {

public:

  // Constructor.
  Nucleus2gamma2(int idBeamIn) : PDF(idBeamIn) {}

  // Update the photon flux.
  void xfUpdate(int , double x, double ) {

    // Minimum impact parameter (~2*radius) [fm].
    // double bmin = 2 * 6.636;
    double bmin = 6.636 + 0.7;

    // Charge of the nucleus.
    double z = 82.;

    // Per-nucleon mass for lead.
    double m2 = pow2(0.9314);
    double alphaEM = 0.007297353080;
    double hbarc = 0.197;
    double xi = x * sqrt(m2) * bmin / hbarc;
    double bK0 = besselK0(xi);
    double bK1 = besselK1(xi);
    double intB = xi * bK1 * bK0 - 0.5 * pow2(xi) * ( pow2(bK1) - pow2(bK0) );
    xgamma = 2. * alphaEM * pow2(z) / M_PI * intB;
  }


};


int main(int argc, char* argv[]) {  // input for file name
    
    if (argc < 2) {
        cout << "Error: No output file number provided!" << endl;
        return 1;
    }

    // Convert the argument to a number and use it in the filename
    string fileNumber = argv[1];

    Pythia pythia;
    
     // Print the settings actually changed by this program.
  pythia.readString("Init:showChangedSettings = on");
  pythia.readString("Init:showChangedParticleData = off");
  pythia.readString("Next:numberCount = 10000");
  pythia.readString("Next:numberShowInfo = 0");
  pythia.readString("Next:numberShowProcess = 0");
  pythia.readString("Next:numberShowEvent = 0");

  pythia.readString("Random:setSeed = on");
  pythia.readString("Random:seed = 0");

  // p-Pb beam energies per nucleon. Beam B is represented by a proton-like
  // beam carrying the external Pb photon flux, as in your previous setup.
  pythia.readString("Beams:frameType = 2");
  pythia.readString("Beams:idA = 2212"); //+z direction
  pythia.readString("Beams:idB = 2212"); //-z  
  pythia.readString("Beams:eA = 6500.0");
  pythia.readString("Beams:eB = 2562.5");

  // Photon from beam B.
  pythia.readString("PDF:beamB2gamma = on");
  pythia.readString("PDF:proton2gammaSet = 0");
  pythia.readString("PDF:beam2gammaApprox = 2");
  pythia.readString("Photon:sampleQ2 = off");  //This is because we're not using pronton flux,"Q2 integrated flux".

  PDFPtr fluxB = make_shared<Nucleus2gamma2>(2212);
  pythia.setPhotonFluxPtr(nullptr, fluxB);

  // Keep only the resolved/hadron-like photon contribution. For a gamma-hadron
  // collision this activates the VMD treatment of the resolved photon.
  pythia.readString("Photon:ProcessType = 1"); // 0 is Mix of resolved-resolved, resolved- direct, direct-resolved,  direct- direct .//1 resolved
    						  // 3 is the case direct-resolved. 

  // Generate only elastic scattering of the sampled VMD state on the proton.
  // SoftQCD elastic has PYTHIA process code 102.
  pythia.readString("SoftQCD:elastic = on");

  pythia.readString("113:mayDecay = off");
  pythia.readString("223:mayDecay = off");
  pythia.readString("333:mayDecay = off");
  

  // Force every generated J/psi to decay only to mu+ mu-.
  pythia.readString("443:mayDecay = on");
  pythia.readString("443:onMode = off");
  pythia.readString("443:onIfMatch = 13 -13");

 
    
    int numEvent = 1000;
    
    pythia.init();
    
    string fileName = "pPb_8p16TeV_pthat10_vmd_jpsi_file_" + fileNumber + ".lhe";

    //string fileName = "/eos/cms/store/group/phys_heavyions/sarteaga/lhe_files_mc/pPb_vmd_Jpsi/pPb_8p16TeV_pthat10_vmd_jpsi_file_" + fileNumber + ".lhe";
    
    // To create LHEF files 
    LHEF3FromPythia8 myLHEF3(&pythia.event, &pythia.info);
   
    myLHEF3.openLHEF(fileName);

    myLHEF3.setInit();
    myLHEF3.initLHEF();
    
    // Begin event loop. Generate event.
    for (int iEvent = 0; iEvent < numEvent; ++iEvent) {

        if (!pythia.next()) continue;
        // Save events in the LHEF
        myLHEF3.setEvent();
        // Write out this event info on the file
        //myLHEF3.eventLHEF();
        
 
    }
    // Show statistics.
    pythia.stat();
    
    myLHEF3.closeLHEF(true);
    cout << "Finishing ... " << endl;
    

    // Done.
    return 0;
}
