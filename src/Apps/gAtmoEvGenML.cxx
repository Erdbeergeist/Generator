//________________________________________________________________________________________
/*!

\program gevgenML

\brief   A GENIE atmospheric neutrino event generation application.

         *** Synopsis :

           gevgenML [-h]
                      [-r run#]
                      -n nev
                      -E energy (or energy range)
                      -p neutrino_pdg
                      -t target_pdg
                      [-f flux_description]
                       [-R coordinate_rotation_matrix]
                       [-t geometry_top_volume_name]
                       [-m max_path_lengths_xml_file]
                       [-L geometry_length_units]
                       [-D geometry_density_units]
                       <-n n_of_events,
                        -e exposure_in_kton_x_yrs >
                        -E min_energy,max_energy
                       [-o output_event_file_prefix]
                       [--seed random_number_seed]
                       [--cross-sections xml_file]
                       [--event-generator-list list_name]
                       [--tune genie_tune]
                       [--message-thresholds xml_file]
                       [--unphysical-event-mask mask]
                       [--event-record-print-level level]
                       [--mc-job-status-refresh-rate  rate]
                       [--cache-file root_file]

         *** Options :

           [] Denotes an optional argument.
           <> Denotes a set of arguments out of which only one can be set.

           -h
              Prints-out help on using gevgen and exits.
           -n
              Specifies the number of events to generate.
           -r
              Specifies the MC run number.
           -e
              Specifies the neutrino energy.
              If what follows the -e option is a comma separated pair of values
              it will be interpreted as an energy range for the flux specified
              via the -f option (see below).
           -p
              Specifies the neutrino PDG code.
           -t
              Specifies the target PDG code (pdg format: 10LZZZAAAI) _or_ a target
              mix (pdg codes with corresponding weights) typed as a comma-separated
              list of pdg codes with the corresponding weight fractions in brackets,
              eg code1[fraction1],code2[fraction2],...
              For example, to use a target mix of 95% O16 and 5% H type:
              `-t 1000080160[0.95],1000010010[0.05]'.
           -f
              Specifies the neutrino flux spectrum.
              It can be any of:
              -- A function:
                 eg ` -f x*x+4*exp(-x)'
              -- A vector file:
                 The vector file should contain 2 columns corresponding to
                 energy,flux (see $GENIE/data/flux/ for few examples).
              -- A 1-D ROOT histogram (TH1D):
                 The general syntax is `-f /full/path/file.root,object_name'
           -g
              Input 'geometry'.
              This option can be used to specify any of:
              1 > A ROOT file containing a ROOT/GEANT geometry description
                  [Examples]
                  - To use the master volume from the ROOT geometry stored
                    in the nd280-geom.root file, type:
                    '-g /some/path/nd280-geom.root'
              2 > A mix of target materials, each with its corresponding weight,
                  typed as a comma-separated list of nuclear pdg codes (in the
                  std PDG2006 convention: 10LZZZAAAI) with the weight fractions
                  in brackets, eg code1[fraction1],code2[fraction2],...
                  If that option is used (no detailed input geometry description)
                  then the interaction vertices are distributed in the detector
                  by the detector MC.
                  [Examples]
                  - To use a target mix of 89% O16 and 11% H, type:
                    '-g 1000080160[0.89],1000010010[0.11]'
                  - To use a target which is 100% C12, type:
                    '-g 1000060120'
           -R
              Input rotation matrix for transforming the flux neutrino coordinates
              from the default Topocentric Horizontal (see GENIE manual) coordinate
              system to the user-defined topocentric coordinate system.
              The rotation is specified by the 3 Euler angles (phi, theta, psi).
              The Euler angles are input as a comma separated list as:
              `-R <convention>:phi,theta,psi',
              where <convention> is either X (for X-convention), Y (for Y-convention),
              X^-1 or Y^-1 (as previously, but using the inverse matrix).
              By default, the X-convention (rotation about Z-axis, then about the
              new X-axis, then about the Z-axis) is used.
              Notes:
              - (Extract from TRotation documentation)
               "Euler angles usually define the rotation of the new coordinate
                system with respect to the original system, however, the TRotation
                class specifies the rotation of the object in the original system
                (an active rotation). To recover the usual Euler rotations (ie.
                rotate the system not the object), you must take the inverse of
                the rotation."
              Examples:
              1. To set the Euler angles phi=3.14, theta=1.28, psi=1.0 using the
                 X-convention, type: `-R 3.14,1.28,1.0', or `-R X:3.14,1.28,1.0'
              2. To set the Euler angles phi=3.14, theta=1.28, psi=1.0 using the
                 Y-convention, type: `-R Y:3.14,1.28,1.0'
              3. To set the Euler angles phi=3.14, theta=1.28, psi=1.0 using the
                 Y-convention, and then use the inverse rotation matrix, type:
                 `-R Y^-1:3.14,1.28,1.0'
           -L
              Input geometry length units, eg 'm', 'cm', 'mm', ...
              [default: 'mm']
           -D
              Input geometry density units, eg 'g_cm3', 'clhep_def_density_unit',...
              [default: 'g_cm3']
           -t
              Input 'top volume' for event generation -
              can be used to force event generation in given sub-detector
              [default: the 'master volume' of the input geometry]
              You can also use the -t option to switch generation on/off at
              multiple volumes as, for example, in:
              `-t +Vol1-Vol2+Vol3-Vol4',
              `-t "+Vol1 -Vol2 +Vol3 -Vol4"',
              `-t -Vol2-Vol4+Vol1+Vol3',
              `-t "-Vol2 -Vol4 +Vol1 +Vol3"'
              where:
              "+Vol1" and "+Vol3" tells GENIE to `switch on'  Vol1 and Vol3, while
              "-Vol2" and "-Vol4" tells GENIE to `switch off' Vol2 and Vol4.
              If the very first character is a '+', GENIE will neglect all volumes
              except the ones explicitly turned on. Vice versa, if the very first
              character is a `-', GENIE will keep all volumes except the ones
              explicitly turned off (feature contributed by J.Holeczek).
           -n
              Specifies how many events to generate.
           -e
              Specifies requested exposure in terms of kton*yrs.
           -E
              Specifies the neutrino energy in GeV.
              Must be a comma-separated pair of numbers, eg `-E 0.3,70'
              [default: 0.5,50]
           -o
              Sets the prefix of the output event file.
              The output filename is built as:
              [prefix].[run_number].[event_tree_format].[file_format]
              The default output filename is:
              gntp.[run_number].ghep.root
              This cmd line arguments lets you override 'gntp'
           --seed
              Random number seed.
           --cross-sections
              Name (incl. full path) of an XML file with pre-computed
              cross-section values used for constructing splines.
           --tune
              Specifies a GENIE comprehensive neutrino interaction model tune.
              [default: "Default"].
           --message-thresholds
              Allows users to customize the message stream thresholds.
              The thresholds are specified using an XML file.
              See $GENIE/config/Messenger.xml for the XML schema.
              Multiple files, delimited with a `:' can be specified.
           --unphysical-event-mask
              Allows users to specify a 16-bit mask to allow certain types of
              unphysical events to be written in the output file.
              [default: all unphysical events are rejected]
           --event-record-print-level
              Allows users to set the level of information shown when the event
              record is printed in the screen. See GHepRecord::Print().
           --mc-job-status-refresh-rate
              Allows users to customize the refresh rate of the status file.
           --cache-file
              Allows users to specify a cache file so that the cache can be
              re-used in subsequent MC jobs.

         *** Examples:

           (1) Generate 100k events (run number 999210) in the energy range 1-10 GeV
               for nu_e and nu_mu only, using the sdave_numu07.dat FLUKA flux file for
               nu_mu and the sdave_nue07.dat file for nu_e (files in /data/flx/).
               Use the detector geometry in the /data/geo/SuperK.root file, where the
               geometry length and density units are m and kgr/m^3. Generate events over
               the entire geometry volume. Pre-computed cross-section data are loaded
               from /data/xsec.xml.

               % gevgenML -r 999210 -n 100000 -E 1,10
                       -f FLUKA:/data/flx/sdave_numu07.dat[14],/data/flx/sdave_nue07.dat[12]
                       -g /data/geo/SuperK.root -L "m" -D "kg_m3"
                       --cross-sections /data/xsec.xml

           (2) Like above but, instead of generating events in a realistic detector
               geometry, use a simple target mix (88.79% O16 + 11.21% H, i.e. `water')

               % gevgenML -r 999210 -n 100000 -E 1,10
                       -f /data/flux/sdave_numu07.dat[14],/data/flux/sdave_nue07.dat[12]
                       -g 1000080160[0.8879],1000010010[0.1121]
                       --cross-sections /data/xsec.xml

                ... to add more

         Please read the GENIE User Manual for more information.

\created August 20, 2010

\author  Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
         University of Liverpool & STFC Rutherford Appleton Lab

         Torben Ferber <torben.ferber \at DESY.DE>
         DESY

         Hugh Gallagher <hugh.gallagher \at stfc.ac.uk>
         Tufts University

         Tarak Thakore <tarak \at mailhost.tifr.res.in>
         Tata Institute of Fundamental Research

         Modified by Lukas Mittnacht no association w/ GENIE Collab

\cpright Copyright (c) 2003-2018, The GENIE Collaboration
         For the full text of the license visit http://copyright.genie-mc.org
         or see $GENIE/LICENSE
*/
//_________________________________________________________________________________________

#include <cassert>
#include <cstdlib>
#include <cctype>
#include <string>
#include <vector>
#include <sstream>
#include <map>

#include <TRotation.h>
#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>
#include <TVector3.h>
#include <TH1.h>
#include <TF1.h>

#include "Framework/Conventions/Units.h"
#include "Framework/EventGen/EventRecord.h"
#include "Framework/EventGen/GFluxI.h"
#include "Framework/EventGen/GMCJDriver.h"
#include "Framework/EventGen/GMCJMonitor.h"
#include "Framework/Messenger/Messenger.h"
#include "Framework/Ntuple/NtpWriter.h"
#include "Framework/Ntuple/NtpMCFormat.h"
#include "Framework/Numerical/RandomGen.h"
#include "Framework/ParticleData/PDGCodes.h"
#include "Framework/ParticleData/PDGLibrary.h"
#include "Framework/Utils/XSecSplineList.h"
#include "Framework/Utils/StringUtils.h"
#include "Framework/Utils/SystemUtils.h"
#include "Framework/Utils/UnitUtils.h"
#include "Framework/Utils/CmdLnArgParser.h"
#include "Framework/Utils/PrintUtils.h"
#include "Framework/Utils/AppInit.h"
#include "Framework/Utils/RunOpt.h"

#include "Framework/Utils/AppInit.h"
#include "Framework/Numerical/Spline.h"
#include "Framework/ParticleData/PDGUtils.h"
#include "Framework/Conventions/GBuild.h"
#include "Framework/Conventions/Controls.h"


#ifdef __GENIE_GEOM_DRIVERS_ENABLED__
#include "Tools/Geometry/GeoUtils.h"
#include "Tools/Geometry/ROOTGeomAnalyzer.h"
#include "Tools/Geometry/PointGeomAnalyzer.h"
#endif

#ifdef __GENIE_FLUX_DRIVERS_ENABLED__
#ifdef __GENIE_GEOM_DRIVERS_ENABLED__
#define __CAN_GENERATE_EVENTS_USING_A_FLUX_OR_TGTMIX__
#include "Tools/Flux/GCylindTH1MLFlux.h"
#include "Tools/Flux/GMonoEnergeticFlux.h"
#include "Tools/Geometry/PointGeomAnalyzer.h"
#endif
#endif


using std::string;
using std::vector;
using std::map;
using std::ostringstream;

using namespace genie;
using namespace genie::flux;

void            GetCommandLineArgs (int argc, char ** argv);
void            PrintSyntax        (void);
#ifdef __CAN_GENERATE_EVENTS_USING_A_FLUX_OR_TGTMIX__
void            GenerateEventsUsingFluxOrTgtMix();
GeomAnalyzerI * GeomDriver              (void);
GFluxI *        FluxDriver              (void);
GFluxI *        MonoEnergeticFluxDriver (void);
GFluxI *        TH1FluxDriver           (void);
#endif
GFluxI *        GetFlux            (void);
GeomAnalyzerI * GetGeometry        (void);

// User-specified options:
//
Long_t          gOptRunNu;                     // run number
string          gOptFluxSim;                   // flux simulation (FLUKA, BGLRS or HAKKM)

map<int,string> gOptFluxFiles;                 // neutrino pdg code -> flux file map
bool            gOptUsingRootGeom = false;     // using root geom or target mix?
map<int,double> gOptTgtMix;                    // target mix  (tgt pdg -> wght frac) / if not using detailed root geom
string          gOptRootGeom;                  // input ROOT file with realistic detector geometry
string          gOptRootGeomTopVol = "";       // input geometry top event generation volume
double          gOptGeomLUnits = 0;            // input geometry length units
double          gOptGeomDUnits = 0;            // input geometry density units
string          gOptExtMaxPlXml;               // max path lengths XML file for input geometry
int             gOptNev = -1;                  // exposure - in terms of number of events
double          gOptKtonYrExposure = -1;       // exposure - in terms of kton*yrs
double          gOptEvMin;                     // minimum neutrino energy
double          gOptEvMax;                     // maximum neutrino energy
string          gOptEvFilePrefix;              // event file prefix
TRotation       gOptRot;                       // coordinate rotation matrix: topocentric horizontal -> user-defined topocentric system
long int        gOptRanSeed;                   // random number seed
string          gOptInpXSecFile;               // cross-section splines

//NEW Vars

double          gOptNuEnergy;     // neutrino E, or min neutrino energy in spectrum
double          gOptNuEnergyRange;// energy range in input spectrum
int             gOptNuPdgCode;    // neutrino PDG code
string          gOptFlux;         //
bool            gOptWeighted;     //
bool            gOptUsingFluxOrTgtMix = false;
// Defaults:
//
NtpMCFormat_t   kDefOptNtpFormat    = kNFGHEP; // def event tree format
string          kDefOptEvFilePrefix = "gntp";  // def output prefix (override with -o)
string          kDefOptGeomLUnits   = "mm";    // def geom length units (override with -L)
string          kDefOptGeomDUnits   = "g_cm3"; // def geom density units (override with -D)
double          kDefOptEvMin        =  0.5;    // min neutrino energy (override with -E)
double          kDefOptEvMax        = 50.0;    // max neutrino energy (override with -E)

//________________________________________________________________________________________
int main(int argc, char** argv)
{
  // Parse command line arguments
  GetCommandLineArgs(argc,argv);

  if ( ! RunOpt::Instance()->Tune() ) {
    LOG("gmkspl", pFATAL) << " No TuneId in RunOption";
    exit(-1);
  }
  RunOpt::Instance()->BuildTune();

  // Iinitialization of random number generators, cross-section table, messenger, cache etc...
  utils::app_init::MesgThresholds(RunOpt::Instance()->MesgThresholdFiles());
  utils::app_init::CacheFile(RunOpt::Instance()->CacheFile());
  utils::app_init::RandGen(gOptRanSeed);
  utils::app_init::XSecTable(gOptInpXSecFile, true);

  // get flux driver
  //
  GFluxI * flux_driver = FluxDriver();


  // get geometry driver
  GeomAnalyzerI * geom_driver = GetGeometry();

  // create the GENIE monte carlo job driver
  GMCJDriver* mcj_driver = new GMCJDriver;
  mcj_driver->SetEventGeneratorList(RunOpt::Instance()->EventGeneratorList());
  mcj_driver->UseFluxDriver(flux_driver);
  mcj_driver->UseGeomAnalyzer(geom_driver);
  mcj_driver->Configure();
  mcj_driver->UseSplines();
  mcj_driver->ForceSingleProbScale();

  // initialize an ntuple writer
  NtpWriter ntpw(kDefOptNtpFormat, gOptRunNu);
  ntpw.CustomizeFilenamePrefix(gOptEvFilePrefix);
  ntpw.Initialize();

  // Create a MC job monitor for a periodically updated status file
  GMCJMonitor mcjmonitor(gOptRunNu);
  mcjmonitor.SetRefreshRate(RunOpt::Instance()->MCJobStatusRefreshRate());

  // Set GHEP print level
  GHepRecord::SetPrintLevel(RunOpt::Instance()->EventRecordPrintLevel());

  // event loop
  for(int iev = 0; iev < gOptNev; iev++) {


    // generate next event
    EventRecord* event = mcj_driver->GenerateEvent();


    // set weight (if using a weighted flux)
    //event->SetWeight(event->Weight()*flux_driver->Weight());

    // print-out
    LOG("gevgenML", pNOTICE) << "Generated event: " << *event;

    // save the event, refresh the mc job monitor
    ntpw.AddEventRecord(iev, event);
    mcjmonitor.Update(iev,event);

    // clean-up
    delete event;
  }

  // save the event file
  ntpw.Save();

  // clean-up
  delete geom_driver;
  delete flux_driver;
  delete mcj_driver;

  return 0;
}

//##############################################################################
//                              NEW FUNCITONS
//##############################################################################
GFluxI * FluxDriver(void)
{
// create & configure one of the generic flux drivers
//
  GFluxI * flux_driver = 0;

  if(gOptNuEnergyRange<0) flux_driver = MonoEnergeticFluxDriver();
  else flux_driver = TH1FluxDriver();

  return flux_driver;
}
//____________________________________________________________________________
GFluxI * MonoEnergeticFluxDriver(void)
{
//
//
  flux::GMonoEnergeticFlux * flux =
              new flux::GMonoEnergeticFlux(gOptNuEnergy, gOptNuPdgCode);
  GFluxI * flux_driver = dynamic_cast<GFluxI *>(flux);
  return flux_driver;
}
GFluxI * TH1FluxDriver(void)
{
//
//
  flux::GCylindTH1MLFlux * flux = new flux::GCylindTH1MLFlux;
  TH1D * spectrum = 0;

  int flux_entries = 100000;

  double emin = gOptNuEnergy;
  double emax = gOptNuEnergy+gOptNuEnergyRange;
  double de   = gOptNuEnergyRange;

  // check whether the input flux is a file or a functional form
  //
  bool input_is_text_file = ! gSystem->AccessPathName(gOptFlux.c_str());
  bool input_is_root_file = gOptFlux.find(".root") != string::npos &&
                            gOptFlux.find(",") != string::npos;
  if(input_is_text_file) {
    //
    // ** generate the flux histogram from the x,y pairs in the input text file
    //
    Spline * input_flux = new Spline(gOptFlux.c_str());
    int  n = 100;
    double estep = (emax-emin)/(n-1);
    double ymax  = -1, ry = -1, gy = -1, e = -1;
    for(int i=0; i<n; i++) {
      e = emin + i*estep;
      ymax = TMath::Max(ymax, input_flux->Evaluate(e));
    }
    ymax *= 1.3;

    RandomGen * r = RandomGen::Instance();
    spectrum  = new TH1D("spectrum","neutrino flux", 300, emin, emax);
    spectrum->SetDirectory(0);

    for(int ientry=0; ientry<flux_entries; ientry++) {
      bool accept = false;
      unsigned int iter=0;
      while(!accept) {
        iter++;
        //FIXME DIRTY FIX SHOULD BE FIXED BY INCLUDING conventions/controls.h but does not work
        if(iter > 1000) {
           LOG("gevgenML", pFATAL) << "Couldn't generate a flux histogram";
           exit(1);
        }
        e = emin + de * r->RndGen().Rndm();
        gy = ymax * r->RndGen().Rndm();
        ry = input_flux->Evaluate(e);
        accept = gy < ry;
        if(accept) spectrum->Fill(e);
      }
    }
    delete input_flux;
  }
  else if(input_is_root_file) {
    //
    // ** extract specified flux histogram from the input root file
    //
    vector<string> fv = utils::str::Split(gOptFlux,",");
    assert(fv.size()==2);
    assert( !gSystem->AccessPathName(fv[0].c_str()) );

    LOG("gevgenML", pNOTICE) << "Getting input flux from root file: " << fv[0];
    TFile * flux_file = new TFile(fv[0].c_str(),"read");

    LOG("gevgenML", pNOTICE) << "Flux name: " << fv[1];
    TH1D * hst = (TH1D *)flux_file->Get(fv[1].c_str());
    assert(hst);

    LOG("gevgenML", pNOTICE) << hst->GetEntries();

    // Copy in the flux histogram from the root file and remove bins outside the emin,emax range
    spectrum = (TH1D*)hst->Clone();
    spectrum->SetNameTitle("spectrum","neutrino_flux");
    spectrum->SetDirectory(0);
    for(int ibin = 1; ibin <= hst->GetNbinsX(); ibin++) {
      if(hst->GetBinLowEdge(ibin) + hst->GetBinWidth(ibin) > emax ||
         hst->GetBinLowEdge(ibin) < emin) {
        spectrum->SetBinContent(ibin, 0);
      }
    }

    LOG("gevgenML", pNOTICE) << spectrum->GetEntries();

    flux_file->Close();
    delete flux_file;

    LOG("gevgenML", pNOTICE) << spectrum->GetEntries();

  } else {
    //
    // ** generate the flux histogram from the input functional form
    //
    TF1 *  input_func = new TF1("input_func", gOptFlux.c_str(), emin, emax);
    spectrum  = new TH1D("spectrum","neutrino flux", 300, emin, emax);
    spectrum->SetDirectory(0);
    spectrum->FillRandom("input_func", flux_entries);
    delete input_func;
  }
  // save input flux

  TFile f("./config/fluxes/input-flux.root","recreate");
  spectrum->Write();
  f.Close();

  TVector3 bdir (0,1,0);
  TVector3 bspot(0,0,0);

  flux->SetNuDirection      (bdir);
  flux->SetBeamSpot         (bspot);
  LOG("Setting Transverse RAD", pINFO) << "FINDME";
  flux->SetTransverseRadius (500);
  flux->AddEnergySpectrum   (gOptNuPdgCode, spectrum);

  GFluxI * flux_driver = dynamic_cast<GFluxI *>(flux);
  return flux_driver;
}

//##############################################################################


//________________________________________________________________________________________
GeomAnalyzerI* GetGeometry(void)
{
  GeomAnalyzerI * geom_driver = 0;

#ifdef __GENIE_GEOM_DRIVERS_ENABLED__

  if(gOptUsingRootGeom) {
    //
    // *** Using a realistic root-based detector geometry description
    //

    // creating & configuring a root geometry driver
    geometry::ROOTGeomAnalyzer * rgeom =
            new geometry::ROOTGeomAnalyzer(gOptRootGeom);
    rgeom -> SetLengthUnits  (gOptGeomLUnits);
    rgeom -> SetDensityUnits (gOptGeomDUnits);
    rgeom -> SetTopVolName   (gOptRootGeomTopVol);
    // getting the bounding box dimensions along z so as to set the
    // appropriate upstream generation surface for the JPARC flux driver
    TGeoVolume * topvol = rgeom->GetGeometry()->GetTopVolume();
    if(!topvol) {
      LOG("gevgenML", pFATAL) << " ** Null top ROOT geometry volume!";
      gAbortingInErr = true;
      exit(1);
    }
/*
    TGeoShape * bounding_box = topvol->GetShape();
    bounding_box->GetAxisRange(3, zmin, zmax);
    zmin *= rgeom->LengthUnits();
    zmax *= rgeom->LengthUnits();
*/
    // switch on/off volumes as requested
    if ( (gOptRootGeomTopVol[0] == '+') || (gOptRootGeomTopVol[0] == '-') ) {
      bool exhaust = (*gOptRootGeomTopVol.c_str() == '+');
      utils::geometry::RecursiveExhaust(topvol, gOptRootGeomTopVol, exhaust);
    }

    // casting to the GENIE geometry driver interface
    geom_driver = dynamic_cast<GeomAnalyzerI *> (rgeom);
  }
  else {
    //
    // *** Using a 'point' geometry with the specified target mix
    // *** ( = a list of targets with their corresponding weight fraction)
    //

    // creating & configuring a point geometry driver
    geometry::PointGeomAnalyzer * pgeom =
              new geometry::PointGeomAnalyzer(gOptTgtMix);
    // casting to the GENIE geometry driver interface
    geom_driver = dynamic_cast<GeomAnalyzerI *> (pgeom);
  }

#else
  LOG("gevgenML", pFATAL) << "You need to enable the GENIE geometry drivers first!";
  LOG("gevgenML", pFATAL) << "Use --enable-geom-drivers at the configuration step.";
  gAbortingInErr = true;
  exit(1);
#endif

  return geom_driver;
}

//________________________________________________________________________________________
void GetCommandLineArgs(int argc, char ** argv)
{
// Get the command line arguments

  RunOpt::Instance()->ReadFromCommandLine(argc,argv);

  LOG("gevgenML", pNOTICE) << "Parsing command line arguments";

  CmdLnArgParser parser(argc,argv);

  // help?
  bool help = parser.OptionExists('h');
  if(help) {
      PrintSyntax();
      exit(0);
  }

  //
  // *** run number
  //
  if( parser.OptionExists('r') ) {
    LOG("gevgenML", pDEBUG) << "Reading MC run number";
    gOptRunNu = parser.ArgAsLong('r');
  } else {
    LOG("gevgenML", pDEBUG) << "Unspecified run number - Using default";
    gOptRunNu = 100000000;
  } //-r

  //
  // *** exposure
  //

  // in number of events
  bool have_required_statistics = false;
  if( parser.OptionExists('n') ) {
    LOG("gevgenML", pDEBUG)
        << "Reading number of events to generate";
    gOptNev = parser.ArgAsInt('n');
    have_required_statistics = true;
  }//-n?
  // or, in kton*yrs
  if( parser.OptionExists('e') ) {
    if(have_required_statistics) {
      LOG("gevgenML", pFATAL)
         << "Can't request exposure both in terms of number of events and  kton*yrs"
         << "\nUse just one of the -n and -e options";
      PrintSyntax();
      gAbortingInErr = true;
      exit(1);
    }
    LOG("gevgenML", pDEBUG)
        << "Reading requested exposure in kton*yrs";
    gOptKtonYrExposure = parser.ArgAsDouble('e');
    have_required_statistics = true;
  }//-e?
  if(!have_required_statistics) {
    LOG("gevgenML", pFATAL)
       << "You must request exposure either in terms of number of events and  kton*yrs"
       << "\nUse any of the -n, -e options";
    PrintSyntax();
    gAbortingInErr = true;
    exit(1);
  }

  //
  // *** event file prefix
  //
  if( parser.OptionExists('o') ) {
    LOG("gevgenML", pDEBUG) << "Reading the event filename prefix";
    gOptEvFilePrefix = parser.ArgAsString('o');
  } else {
    LOG("gevgenML", pDEBUG)
      << "Will set the default event filename prefix";
    gOptEvFilePrefix = kDefOptEvFilePrefix;
  } //-o

  // flux functional form
  bool using_flux = false;
  if( parser.OptionExists('f') ) {
    LOG("gevgenML_mod", pINFO) << "Reading flux function";
    gOptFlux = parser.ArgAsString('f');
    using_flux = true;
  }

  // neutrino energy
  if( parser.OptionExists('E') ) {
    LOG("gevgenML_mod", pINFO) << "Reading neutrino energy";
    string nue = parser.ArgAsString('E');

    // is it just a value or a range (comma separated set of values)
    if(nue.find(",") != string::npos) {
       // split the comma separated list
       vector<string> nurange = utils::str::Split(nue, ",");
       assert(nurange.size() == 2);
       double emin = atof(nurange[0].c_str());
       double emax = atof(nurange[1].c_str());
       assert(emax>emin && emin>=0);
       gOptNuEnergy      = emin;
       gOptNuEnergyRange = emax-emin;
       if(!using_flux) {
          LOG("gevgenML", pWARN)
             << "No flux was specified but an energy range was input!";
          LOG("gevgenML", pWARN)
             << "Events will be generated at fixed E = " << gOptNuEnergy << " GeV";
          gOptNuEnergyRange = -1;
       }
    } else {
       gOptNuEnergy       = atof(nue.c_str());
       gOptNuEnergyRange = -1;
    }
  } else {
    LOG("gevgenML", pFATAL) << "Unspecified neutrino energy - Exiting";
    PrintSyntax();
    exit(1);
  }

  // neutrino PDG code
  if( parser.OptionExists('p') ) {
    LOG("gevgenML", pINFO) << "Reading neutrino PDG code";
    gOptNuPdgCode = parser.ArgAsInt('p');
  } else {
    LOG("gevgenML", pFATAL) << "Unspecified neutrino PDG code - Exiting";
    PrintSyntax();
    exit(1);
  }

  //
  // *** geometry
  //
  string geom = "";
  string lunits, dunits;
  if( parser.OptionExists('g') ) {
    LOG("gevgenML", pDEBUG) << "Getting input geometry";
    geom = parser.ArgAsString('g');

    // is it a ROOT file that contains a ROOT geometry?
    bool accessible_geom_file =
        utils::system::FileExists(geom.c_str());
    if (accessible_geom_file) {
      gOptRootGeom      = geom;
      gOptUsingRootGeom = true;
    }
  } else {
      LOG("gevgenML", pFATAL)
        << "No geometry option specified - Exiting";
      PrintSyntax();
      gAbortingInErr = true;
      exit(1);
  } //-g

  if(gOptUsingRootGeom) {
     // using a ROOT geometry - get requested geometry units

     // legth units:
     if( parser.OptionExists('L') ) {
        LOG("gevgenML", pDEBUG)
           << "Checking for input geometry length units";
        lunits = parser.ArgAsString('L');
     } else {
        LOG("gevgenML", pDEBUG) << "Using default geometry length units";
        lunits = kDefOptGeomLUnits;
     } // -L
     // density units:
     if( parser.OptionExists('D') ) {
        LOG("gevgenML", pDEBUG)
           << "Checking for input geometry density units";
        dunits = parser.ArgAsString('D');
     } else {
        LOG("gevgenML", pDEBUG) << "Using default geometry density units";
        dunits = kDefOptGeomDUnits;
     } // -D
     gOptGeomLUnits = genie::utils::units::UnitFromString(lunits);
     gOptGeomDUnits = genie::utils::units::UnitFromString(dunits);

     // check whether an event generation volume name has been
     // specified -- default is the 'top volume'
     if( parser.OptionExists('t') ) {
        LOG("gevgenML", pDEBUG) << "Checking for input volume name";
        gOptRootGeomTopVol = parser.ArgAsString('t');
     } else {
        LOG("gevgenML", pDEBUG) << "Using the <master volume>";
     } // -t

     // check whether an XML file with the maximum (density weighted)
     // path lengths for each detector material is specified -
     // otherwise will compute the max path lengths at job init
     if( parser.OptionExists('m') ) {
        LOG("gevgenML", pDEBUG)
              << "Checking for maximum path lengths XML file";
        gOptExtMaxPlXml = parser.ArgAsString('m');
     } else {
        LOG("gevgenML", pDEBUG)
           << "Will compute the maximum path lengths at job init";
        gOptExtMaxPlXml = "";
     } // -m
  } // using root geom?

  else {
    // User has specified a target mix.
    // Decode the list of target pdf codes & their corresponding weight fraction
    // (specified as 'pdg_code_1[fraction_1],pdg_code_2[fraction_2],...')
    // See documentation on top section of this file.
    //
    gOptTgtMix.clear();
    vector<string> tgtmix = utils::str::Split(geom,",");
    if(tgtmix.size()==1) {
         int    pdg = atoi(tgtmix[0].c_str());
         double wgt = 1.0;
         gOptTgtMix.insert(map<int, double>::value_type(pdg, wgt));
    } else {
      vector<string>::const_iterator tgtmix_iter = tgtmix.begin();
      for( ; tgtmix_iter != tgtmix.end(); ++tgtmix_iter) {
         string tgt_with_wgt = *tgtmix_iter;
         string::size_type open_bracket  = tgt_with_wgt.find("[");
         string::size_type close_bracket = tgt_with_wgt.find("]");
         if (open_bracket ==string::npos ||
             close_bracket==string::npos)
         {
             LOG("gevgenML", pFATAL)
                << "You made an error in specifying the target mix";
             PrintSyntax();
             gAbortingInErr = true;
             exit(1);
         }
         string::size_type ibeg = 0;
         string::size_type iend = open_bracket;
         string::size_type jbeg = open_bracket+1;
         string::size_type jend = close_bracket;
         int    pdg = atoi(tgt_with_wgt.substr(ibeg,iend-ibeg).c_str());
         double wgt = atof(tgt_with_wgt.substr(jbeg,jend-jbeg).c_str());
         LOG("gevgenML", pDEBUG)
            << "Adding to target mix: pdg = " << pdg << ", wgt = " << wgt;
         gOptTgtMix.insert(map<int, double>::value_type(pdg, wgt));

      }// tgtmix_iter
    } // >1 materials in mix
  } // using tgt mix?

  //
  // Coordinate rotation matrix
  //
  gOptRot.SetToIdentity();
  if( parser.OptionExists('R') ) {
    string rotarg = parser.ArgAsString('R');
    //get convention
    string::size_type j = rotarg.find_first_of(":",0);
    string convention = "";
    if(j==string::npos) { convention = "X"; }
    else                { convention = rotarg.substr(0,j); }
    //get angles phi,theta,psi
    rotarg.erase(0,j+1);
    vector<string> euler_angles = utils::str::Split(rotarg,",");
    if(euler_angles.size() != 3) {
       LOG("gevgenML", pFATAL)
         << "You didn't specify all 3 Euler angles using the -R option";
       PrintSyntax();
       gAbortingInErr = true;
       exit(1);
    }
    double phi   = atof(euler_angles[0].c_str());
    double theta = atof(euler_angles[1].c_str());
    double psi   = atof(euler_angles[2].c_str());
    //set Euler angles using appropriate convention
    if(convention.find("X")!=string::npos ||
       convention.find("x")!=string::npos)
    {
       LOG("gevgenML", pNOTICE) << "Using X-convention for input Euler angles";
       gOptRot.SetXEulerAngles(phi,theta,psi);
    } else
    if(convention.find("Y")!=string::npos ||
       convention.find("y")!=string::npos)
    {
       LOG("gevgenML", pNOTICE) << "Using Y-convention for input Euler angles";
       gOptRot.SetYEulerAngles(phi,theta,psi);
    } else {
       LOG("gevgenML", pFATAL)
         << "Unknown Euler angle convention. Please use the X- or Y-convention";
       PrintSyntax();
       gAbortingInErr = true;
       exit(1);
    }
    //invert?
    if(convention.find("^-1")!=string::npos) {
       LOG("gevgenML", pNOTICE) << "Inverting rotation matrix";
       gOptRot.Invert();
    }
  }

  //
  // *** random number seed
  //
  if( parser.OptionExists("seed") ) {
    LOG("gevgenML", pINFO) << "Reading random number seed";
    gOptRanSeed = parser.ArgAsLong("seed");
  } else {
    LOG("gevgenML", pINFO) << "Unspecified random number seed - Using default";
    gOptRanSeed = -1;
  }

  //
  // *** input cross-section file
  //
  if( parser.OptionExists("cross-sections") ) {
    LOG("gevgenML", pINFO) << "Reading cross-section file";
    gOptInpXSecFile = parser.ArgAsString("cross-sections");
  } else {
    LOG("gevgenML", pINFO) << "Unspecified cross-section file";
    gOptInpXSecFile = "";
  }

  //
  // print-out summary
  //

  PDGLibrary * pdglib = PDGLibrary::Instance();

  ostringstream gminfo;
  if (gOptUsingRootGeom) {
    gminfo << "Using ROOT geometry - file: " << gOptRootGeom
           << ", top volume: "
           << ((gOptRootGeomTopVol.size()==0) ? "<master volume>" : gOptRootGeomTopVol)
           << ", max{PL} file: "
           << ((gOptExtMaxPlXml.size()==0) ? "<none>" : gOptExtMaxPlXml)
           << ", length  units: " << lunits
           << ", density units: " << dunits;
  } else {
    gminfo << "Using target mix - ";
    map<int,double>::const_iterator iter;
    for(iter = gOptTgtMix.begin(); iter != gOptTgtMix.end(); ++iter) {
          int    pdg_code = iter->first;
          double wgt      = iter->second;
          TParticlePDG * p = pdglib->Find(pdg_code);
          if(p) {
            string name = p->GetName();
            gminfo << "(" << name << ") -> " << 100*wgt << "% / ";
          }//p?
    }
  }

  ostringstream fluxinfo;
  fluxinfo << "Using " << gOptFluxSim << " flux files: ";
  map<int,string>::const_iterator file_iter = gOptFluxFiles.begin();
  for( ; file_iter != gOptFluxFiles.end(); ++file_iter) {
     int neutrino_code = file_iter->first;
     string filename   = file_iter->second;
     TParticlePDG * p = pdglib->Find(neutrino_code);
     if(p) {
        string name = p->GetName();
        fluxinfo << "(" << name << ") -> " << filename << " / ";
     }
  }

  ostringstream expinfo;
  if(gOptNev > 0)            { expinfo << gOptNev            << " events";   }
  if(gOptKtonYrExposure > 0) { expinfo << gOptKtonYrExposure << " kton*yrs"; }

  ostringstream rotation;
  rotation << "\t| " <<  gOptRot.XX() << "  " << gOptRot.XY() << "  " << gOptRot.XZ() << " |\n";
  rotation << "\t| " <<  gOptRot.YX() << "  " << gOptRot.YY() << "  " << gOptRot.YZ() << " |\n";
  rotation << "\t| " <<  gOptRot.ZX() << "  " << gOptRot.ZY() << "  " << gOptRot.ZZ() << " |\n";

  LOG("gevgenML", pNOTICE)
     << "\n\n"
     << utils::print::PrintFramedMesg("gevgenML job configuration");

  LOG("gevgenML", pNOTICE)
     << "\n"
     << "\n @@ Run number: " << gOptRunNu
     << "\n @@ Random number seed: " << gOptRanSeed
     << "\n @@ Using cross-section file: " << gOptInpXSecFile
     << "\n @@ Geometry"
     << "\n\t" << gminfo.str()
     << "\n @@ Flux"
     << "\n\t" << fluxinfo.str()
     << "\n @@ Exposure"
     << "\n\t" << expinfo.str()
     << "\n @@ Cuts"
     << "\n\t Using energy range = (" << gOptEvMin << " GeV, " << gOptEvMax << " GeV)"
     << "\n @@ Coordinate transformation (Rotation THZ -> User-defined coordinate system)"
     << "\n" << rotation.str()
     << "\n\n";

  //
  // final checks
  //
  if(gOptKtonYrExposure > 0) {
    LOG("gevgenML", pFATAL)
      << "\n Option to set exposure in terms of kton*yrs not supported just yet!"
      << "\n Try the -n option instead";
    PrintSyntax();
    gAbortingInErr = true;
    exit(1);
  }
}
//________________________________________________________________________________________
void PrintSyntax(void)
{
  LOG("gevgenML", pFATAL)
   << "\n **Syntax**"
   << "\n gevgenML [-h]"
   << "\n           [-r run#]"
   << "\n            -n nev"
   << "\n            -g geometry"
   << "\n            -E energy (or energy range) "
   << "\n            -p neutrino_pdg"
   << "\n           [-R coordinate_rotation_matrix]"
   << "\n           [-t geometry_top_volume_name]"
   << "\n           [-m max_path_lengths_xml_file]"
   << "\n           [-L geometry_length_units]"
   << "\n           [-D geometry_density_units]"
   << "\n           <-n n_of_events,"
   << "\n            -e exposure_in_kton_x_yrs>"
   << "\n           [-o output_event_file_prefix]"
   << "\n           [--seed random_number_seed]"
   << "\n            --cross-sections xml_file"
   << "\n           [--event-generator-list list_name]"
   << "\n           [--message-thresholds xml_file]"
   << "\n           [--unphysical-event-mask mask]"
   << "\n           [--event-record-print-level level]"
   << "\n           [--mc-job-status-refresh-rate  rate]"
   << "\n           [--cache-file root_file]"
   << "\n"
   << " Please also read the detailed documentation at http://www.genie-mc.org"
   << "\n";
}
//________________________________________________________________________________________
