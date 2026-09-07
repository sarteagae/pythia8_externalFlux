//My Main94.cc to produce photo-nuclear dijet production set up 
//Modified from  test78.cc by Ilkka Helenius  and  following main68.cc

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

#include <math.h>
#include <cmath>

#include "Pythia8/Pythia.h"
#include "flux/PbFluxFromTable.h"  //to include photonflux table

using namespace Pythia8;

using namespace std;

/// photon-flux definitioin

/*
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
*/
int main(int argc, char* argv[]) {  // input for file name
    
    if (argc < 2) {
        cout << "Error: No output file number provided!" << endl;
        return 1;
    }

    // Convert the argument to a number and use it in the filename
    string fileNumber = argv[1];

    Pythia pythia;
    
    // Decrease the output.
    pythia.readString("Init:showChangedSettings = on");
    pythia.readString("Init:showChangedParticleData = off");
    pythia.readString("Next:numberCount = 100");
    pythia.readString("Next:numberShowInfo = 0");
    pythia.readString("Next:numberShowProcess = 1");  
    pythia.readString("Next:numberShowEvent = 0");   


    pythia.readString("Random:setSeed = on"); // Change to random seed
    pythia.readString("Random:seed = 0");// value 0 gives a random seed based on the time


    // Beam settings.
    pythia.readString("Beams:frameType = 2");    // to identify two beams
    pythia.readString("Beams:idA = 2212"); //Proton(2212)  
    pythia.readString("Beams:idB = 2212"); //Lead (1000822080) 
    
   

    // p-Pb energy.   
    pythia.readString("Beams:eA = 6500");  // Beam going +z. In this case proton 
    pythia.readString("Beams:eB = 2560");  // Beam going -z . In our case;Pb going left  
    

    pythia.readString("PDF:beamB2gamma = on"); // Enables photon sub-beam from beam particle B so the photon flux is associatetd to the lead. 
    pythia.readString("PDF:proton2gammaSet = 0"); //***Comment this out for the moment Jun12-2026
    pythia.readString("PDF:beam2gammaApprox = 3");// Implimenting new option to read photon-flux table from external source. 
    pythia.readString("Photon:sampleQ2 = off");/// This is because we're not using pronton flux,"Q2 integrated flux".
    
    
    
   // Set up the photon flux.
  

    
    const int pbID = 1000822080;
    const string pbFluxSpline = "data/Pb_flux/pb_xgamma_spline.dat";
    PDFPtr photonFluxPb = make_shared<PbFluxFromTable>(pbID, pbFluxSpline);

// adding flux to the beam that emits the photon. 
    pythia.setPhotonFluxPtr(nullptr, photonFluxPb);

    pythia.readString("MultipartonInteractions:pT0Ref = 3.0"); //
    
     // Photoproduction and relevant hard processes.
    pythia.settings.mode("Photon:ProcessType", 0); // 0 is Mix of resolved-resolved, resolved- direct, direct-resolved,  direct- direct .
    						  // 3 is the case direct-resolved.  
    pythia.readString("PhotonParton:all = on");
    
    pythia.readString("PartonLevel:MPI = off"); 
  
    pythia.readString("HardQCD:all = on"); // comment out for diffractive process.Keep it for photo-gluon interaction
    pythia.readString("PhaseSpace:pTHatMin = 10.0");
     
    
    
    int numEvent = 10000;
    
    pythia.init();

    
    //string fileName = "gamma_proton_pPb_8p16TeV_pthat10_xternal_flux_file_" + fileNumber + ".lhe";
    
    string fileName = "/eos/cms/store/group/phys_heavyions/sarteaga/lhe_files_mc/gamma_proton/external_flux/gamma_photon_pPb_8p16TeV_pthat10_external_PhotonFlux_file_" + fileNumber + ".lhe";
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
        
  //       if (pythia.info.isHardDiffractiveA()) ++nDiffA;
 //        if (pythia.info.isHardDiffractiveB()) ++nDiffB;
    /*s
     cout << pythia.info.code()
     << "  "
     << pythia.info.nameProc()
     << endl;
      */  
    }
    // Show statistics.
    pythia.stat();
    
    myLHEF3.closeLHEF(true);
    cout << "Finishing ... " << endl;


// --- Cross section ---
double sigma_mb = pythia.info.sigmaGen();      // in millibarns
double sigma_nb = sigma_mb * 1e6;             // convert to naobarns

double sigmaErr_mb = pythia.info.sigmaErr();   // statistical error (mb)
double sigmaErr_nb = sigmaErr_mb * 1e6;        // error in fb

cout << "======================================" << endl;
cout << "Generated cross section:" << endl;
cout << "  sigma = " << sigma_mb << " mb" << endl;
cout << "  sigma = " << sigma_nb << " nb" << endl;
cout << "  stat. error = " << sigmaErr_nb << " nb" << endl;
cout << "======================================" << endl;


    
//    cout << "Number of diffractive events in side A = " << nDiffA << endl << endl;
//    cout << "Number of diffractive events in side B = " << nDiffB << endl << endl;
    
    // Done.
    return 0;
}
