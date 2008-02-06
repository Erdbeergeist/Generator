//____________________________________________________________________________
/*!

\program gT2Kevgen

\brief   A GENIE event generation driver 'customized' for T2K.

         This driver can handle the JPARC neutrino flux files generated by
         jnubeam and use the realistic detector geometries / target mix of
         T2K detectors. It can be used for full-blown GENIE event generation
         in nd280, 2km and SuperK.

         T2K users should note that the generic GENIE event generation driver
         (gevgen) may still be a more appropriate tool to use for the simpler      
         event generation cases required for many 4-vector level / systematic
         studies - Please see the GENIE documentation (http://www.genie-mc.org)
         and contact me <C.V.Andreopoulos@rl.ac.uk> if in doubt.

         Syntax :
           gT2Kevgen [-h] -n nev [-r run#] [-d detector] 
                     -f flux -g geometry [-u geometry_units] 

         Options :
           [] Denotes an optional argument
           -h Prints out the gT2Kevgen syntax and exits
           -n Specifies the number of events to generate
           -r Specifies the MC run number (def: 1000)
           -d detector (0: nd280, 1: 2km, 2: SK)
           -g geometry
                This can be used to specify any of:
                * A ROOT file containing a ROOT/GEANT geometry description
                  Examples: 
		     >> to use the master volume from the ROOT geometry stored 
                        in the nd280-geom.root file, type:
                        '-g /some/path/nd280-geom.root'
                  [Note] 
                     This is the standard option for generating events in the
                     nd280 and the 2km detectors.
                * A mix of target materials, each with its corresponding weight,
                  typed as a comma-separated list of nuclear pdg codes (in the
                  std PDG2006 convention: 10LZZZAAAI) with the weight fractions
                  in brackets, eg code1[fraction1],code2[fraction2],...
                  If that option is used (no detailed input geometry description) 
                  then the interaction vertices are distributed in the detector
                  by the detector mc.
                  Examples: 
                     >> to use a target mix of 95% O16 and 5% H type:
                        '-g 1000080160[0.95],1000010010[0.05]'
		     >> to use a target which is 100% C12, type:
                        '-g 1000060120'
                  [Note]                   
                     This is the standard option for generating events in the
                     SuperK detector.
           -u geometry length units [default: meter]
           -f flux 
                This can be used to specify any of:
   	        * A ROOT file with a flux neutrino ntuple generated by the T2K 
                  (jnubeam) beam simulation. See (T2K internal):  
                  See: http://jnusrv01.kek.jp/internal/t2k/nubeam/flux/
                  The original hbook ntuples need to be converted to a ROOT 
                  format using the h2root ROOT utility.   
                  The jnubeam flux ntuples are read via GENIE's GJPARCNuFlux 
                  driver. GENIE passes-through the complete input flux 
                  information (eg parent decay kinematics / position etc) for 
                  each neutrino event it generates (an additional 'flux' branch
                  is added at the output event tree).
                * 

         Examples:
               <to be added>


         You can further control the GENIE behaviour by setting its standard 
         environmental variables, more importantly
          - GEVGL        --> specifies which event generation threads are loaded
          - GSPLOAD      --> specifies the XML cross section table is loaded at init
          - GSEED        --> specifies the GENIE seed
          - GMSGCONF     --> overrides default mesg thresholds 
          - GUSERPHYSOPT --> overrides default table of user physics params.
         and others. 
         Please read the GENIE documentation.

\author  Costas Andreopoulos <C.V.Andreopoulos@rl.ac.uk>
         STFC, Rutherford Appleton Laboratory

\created February 05, 2008

\cpright Copyright (c) 2003-2008, GENIE Neutrino MC Generator Collaboration
         For the full text of the license visit http://copyright.genie-mc.org
         or see $GENIE/LICENSE
*/
//____________________________________________________________________________

#include <cassert>
#include <string>
#include <vector>
#include <map>

#include <TSystem.h>

#include "Conventions/Units.h"
#include "EVGCore/EventRecord.h"
#include "EVGDrivers/GFluxI.h"
#include "EVGDrivers/GMCJDriver.h"
#include "EVGDrivers/GMCJMonitor.h"
#include "Messenger/Messenger.h"
#include "Ntuple/NtpWriter.h"
#include "Ntuple/NtpMCFormat.h"
#include "Utils/XSecSplineList.h"
#include "Utils/StringUtils.h"
#include "Utils/UnitUtils.h"
#include "Utils/CmdLineArgParserUtils.h"
#include "Utils/CmdLineArgParserException.h"

#ifdef __GENIE_FLUX_DRIVERS_ENABLED__
#include "FluxDrivers/GJPARCNuFlux.h"
#endif

#ifdef __GENIE_GEOM_DRIVERS_ENABLED__
#include "Geo/ROOTGeomAnalyzer.h"
#include "Geo/PointGeomAnalyzer.h"
#endif

using std::string;
using std::vector;
using std::map;

using namespace genie;

void GetCommandLineArgs (int argc, char ** argv);
void PrintSyntax        (void);

//Default options (override them using the command line arguments):
int           kDefOptNevents   = 0;       // default n-events to generate
Long_t        kDefOptRunNu     = 0;       // default run number
string        kDefOptGeomUnits = "m";     // default geometry units
NtpMCFormat_t kDefOptNtpFormat = kNFGHEP; // default event tree format

//User-specified options:
int             gOptNevents;                // n-events to generate
Long_t          gOptRunNu;                  // run number
bool            gOptUsingRootGeom  = false; //
bool            gOptUsingJPARCFlux = false; //
string          gOptRootGeom;               // detector geometry ROOT file
string          gOptGeomUnits;              // detector geometry units
double          gOptLUnits    = 0;          //
double          gOptDensUnits = 0;          //
map<int,double> gOptTgtMix;                 //
string          gOptExtMaxPlXml;            // external max path lengths XML file
string          gOptFluxFile;               // flux file

//____________________________________________________________________________
int main(int argc, char ** argv)
{
  // Parse command line arguments
  GetCommandLineArgs(argc,argv);
  
  // Autoload splines (from the XML file pointed at the $GSPLOAD env. var.,
  // if the env. var. has been set)
  XSecSplineList * xspl = XSecSplineList::Instance();
  xspl->AutoLoad();

  //
  // *** Create/configure the flux driver
  //
  flux::GJPARCNuFlux * jparc_flux_driver = new flux::GJPARCNuFlux;
  jparc_flux_driver->LoadFile(gOptFluxFile);

  //
  // *** Create/configure the geometry driver
  //
  GeomAnalyzerI * geom_driver = 0;

  if(gOptUsingRootGeom) {
      // using a realistic root geometry
      geometry::ROOTGeomAnalyzer * rgeom = 
             new geometry::ROOTGeomAnalyzer(gOptRootGeom);
      rgeom -> SetLengthUnits  (gOptLUnits);
      rgeom -> SetDensityUnits (gOptDensUnits);

      geom_driver = dynamic_cast<GeomAnalyzerI *> (rgeom);
  } 
  else {
      // using a 'point' geometry with the specified
      // target mix
      geometry::PointGeomAnalyzer * pgeom = 
  	      new geometry::PointGeomAnalyzer(gOptTgtMix);
      geom_driver = dynamic_cast<GeomAnalyzerI *> (pgeom);
  } 

  //
  // *** Create/configure the event generation driver
  //
  GMCJDriver * mcj_driver = new GMCJDriver;
  mcj_driver->UseFluxDriver(jparc_flux_driver);
  mcj_driver->UseGeomAnalyzer(geom_driver);
  mcj_driver->Configure();
  mcj_driver->UseSplines();
  mcj_driver->ForceSingleProbScale();

  // Initialize an Ntuple Writer to save GHEP records into a TTree
  NtpWriter ntpw(kDefOptNtpFormat, gOptRunNu);
  ntpw.Initialize();

  // Create an MC Job Monitor
  GMCJMonitor mcjmonitor(gOptRunNu);

  // Generate events / print the GHEP record / add it to the ntuple
  int ievent = 0;
  while ( ievent < gOptNevents) {
     LOG("gT2Kevgen", pNOTICE) << " *** Generating event............ " << ievent;

     // generate a single event for neutrinos coming from the specified flux
     EventRecord * event = mcj_driver->GenerateEvent();
     LOG("gT2Kevgen", pINFO) << "Generated Event GHEP Record: " << *event;

     // add event at the output ntuple, refresh the mc job monitor & clean-up
     ntpw.AddEventRecord(ievent, event);
     mcjmonitor.Update(ievent,event);
     ievent++;
     delete event;
  }

  // Save the generated MC events
  ntpw.Save();

  return 0;
}
//____________________________________________________________________________
void GetCommandLineArgs(int argc, char ** argv)
{
// get gT2Kevgen command line arguments

  // help?
  bool help = genie::utils::clap::CmdLineArgAsBool(argc,argv,'h');
  if(help) {
      PrintSyntax();
      exit(0);
  }

  LOG("gT2Kevgen", pNOTICE) << "Parsing command line arguments";

  // number of events:
  try {
    LOG("gT2Kevgen", pINFO) << "Reading number of events to generate";
    gOptNevents = genie::utils::clap::CmdLineArgAsInt(argc,argv,'n');
  } catch(exceptions::CmdLineArgParserException e) {
    if(!e.ArgumentFound()) {
      LOG("gT2Kevgen", pINFO)
            << "Unspecified number of events to generate - Using default";
      gOptNevents = kDefOptNevents;
    }
  }

  // run number:
  try {
    LOG("gT2Kevgen", pINFO) << "Reading MC run number";
    gOptRunNu = genie::utils::clap::CmdLineArgAsInt(argc,argv,'r');
  } catch(exceptions::CmdLineArgParserException e) {
    if(!e.ArgumentFound()) {
      LOG("gT2Kevgen", pINFO) << "Unspecified run number - Using default";
      gOptRunNu = kDefOptRunNu;
    }
  }

  //
  // *** geometry
  //

  string geom = "";
  try {
    LOG("Main", pINFO) << "Getting input geometry";
    geom = genie::utils::clap::CmdLineArgAsString(argc,argv,'g');

    // is it a ROOT file that contains a ROOT geometry?
    bool accessible_geom_file = 
            ! (gSystem->AccessPathName(geom.c_str()));
    if (accessible_geom_file) {
      gOptRootGeom      = geom;
      gOptUsingRootGeom = true;
    }                 
  } catch(exceptions::CmdLineArgParserException e) {
    if(!e.ArgumentFound()) {
      LOG("Main", pFATAL) 
        << "No geometry option specified - Exiting";
      PrintSyntax();
      exit(1);
    }
  }

  if(gOptUsingRootGeom) {
     // using a ROOT geometry - get requested geometry units
     try {
        LOG("Main", pINFO) << "Getting input geometry units";
        gOptGeomUnits =
              genie::utils::clap::CmdLineArgAsString(argc,argv,'u');
     } catch(exceptions::CmdLineArgParserException e) {
        if(!e.ArgumentFound()) {
            LOG("Main", pNOTICE) << "Using default geometry units";
            gOptGeomUnits = kDefOptGeomUnits;
        }
     } // try-catch (-u)
 
     // set the corresponding density units
     gOptLUnits = genie::utils::units::UnitFromString(gOptGeomUnits);
     if(gOptGeomUnits == "cm") 
          gOptDensUnits = genie::units::gram/genie::units::cm3;
     else if(gOptGeomUnits == "m") 
          gOptDensUnits = genie::units::kilogram/genie::units::m3;
     else {
     }
  } // using root geom?
  else {
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
         if(1) {
         }
	 string::size_type ibeg = 0;
	 string::size_type iend = open_bracket;
	 string::size_type jbeg = open_bracket+1;
	 string::size_type jend = close_bracket-1;
         int    pdg = atoi(tgt_with_wgt.substr(ibeg,iend).c_str());
         double wgt = atof(tgt_with_wgt.substr(jbeg,jend).c_str());
         LOG("Main", pNOTICE) 
            << "Adding to target mix: pdg = " << pdg << ", wgt = " << wgt;
         gOptTgtMix.insert(map<int, double>::value_type(pdg, wgt));
      }//tgtmix_iter
    }//>1
 }

  //
  // *** flux 
  // 
  try {
    LOG("Main", pINFO) << "Getting input flux file";
    gOptFluxFile =
             genie::utils::clap::CmdLineArgAsString(argc,argv,'f');
  } catch(exceptions::CmdLineArgParserException e) {
    if(!e.ArgumentFound()) {
      LOG("Main", pFATAL) << "No flux file was specified - Exiting";
      PrintSyntax();
      exit(1);
    }
  }

  // print the command line options
  LOG("gT2Kevgen", pNOTICE) 
     << "Command-line arguments:" 
     << "\n Number of events requested = " << gOptNevents
     << "\n MC Run Number              = " << gOptRunNu
     << "\n Flux file                  = " << gOptFluxFile
     << "\n Geometry opt               = " << geom;
}
//____________________________________________________________________________
void PrintSyntax(void)
{
  LOG("gT2Kevgen", pNOTICE)
    << "\n\n" << "Syntax:" << "\n"
    << "   gT2Kevgen <need to type options> \n";
}
//____________________________________________________________________________
