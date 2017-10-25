
/** Configuration macro for setting common cuts and processes for G3, G4 and Fluka (M. Al-Turany 27.03.2008)
    specific cuts and processes to g3 or g4 should be set in the g3Config.C, g4Config.C or flConfig.C

*/

/***********************************
//Edited on 01.2017 by L. Zanetti
***********************************/

void SetCuts()
{
  // ------>>>> IMPORTANT!!!!
  // For a correct comparison between GEANE and MC (pull distributions) 
  // or for a simulation without the generation of secondary particles:
  // 1. set LOSS = 2, DRAY = 0, BREM = 1
  // 2. set the following cut values: CUTGAM, CUTELE, CUTNEU, CUTHAD, CUTMUO = 1 MeV or less
  //                                  BCUTE, BCUTM, DCUTE, DCUTM, PPCUTM = 10 TeV
  // (For an explanation of the chosen values, please refer to the GEANT User's Guide
  // or to message #5362 in the PandaRoot Forum >> Monte Carlo Engines >> g3Config.C thread)
  // 
  // The default settings refer to a complete simulation which generates and follows also the secondary particles.


  gMC->SetProcess( "PAIR", 1 ); /** pair production*/
  gMC->SetProcess( "COMP", 1 ); /**Compton scattering*/
  gMC->SetProcess( "PHOT", 1 ); /** photo electric effect */
  gMC->SetProcess( "PFIS", 0 ); /**photofission*/
  gMC->SetProcess( "DRAY", 1 ); /**delta-ray*/
  gMC->SetProcess( "ANNI", 1 ); /**annihilation*/
  gMC->SetProcess( "BREM", 1 ); /**bremsstrahlung*/
  gMC->SetProcess( "HADR", 5 ); /**hadronic process*/
  gMC->SetProcess( "MUNU", 1 ); /**muon nuclear interaction*/
  gMC->SetProcess( "DCAY", 1 ); /**decay*/
  gMC->SetProcess( "LOSS", 3 ); /**energy loss*/
  gMC->SetProcess( "MULS", 1 ); /**multiple scattering*/

  Double_t _KeV = 1.0E-6;         // GeV --> 1 KeV
  Double_t _MeV = 1.0E-4;         // GeV --> 1 MeV
  Double_t tofmax = 1.E10;        // seconds
  
  gMC->SetCut( "CUTGAM", 1.*_KeV );   /** gammas (GeV)*/
  gMC->SetCut( "CUTELE", 0.1*_MeV );   /** electrons (GeV)*/
  gMC->SetCut( "CUTNEU", 1.*_KeV );   /** neutral hadrons (GeV)*/
  gMC->SetCut( "CUTHAD", 1.*_KeV );   /** charged hadrons (GeV)*/
  gMC->SetCut( "CUTMUO", 1.*_KeV );   /** muons (GeV)*/
  gMC->SetCut( "BCUTE", 1.*_KeV );    /** electron bremsstrahlung (GeV)*/
  gMC->SetCut( "BCUTM", 1.*_KeV );    /** muon and hadron bremsstrahlung(GeV)*/ 
  gMC->SetCut( "DCUTE", 1.*_KeV );    /** delta-rays by electrons (GeV)*/
  gMC->SetCut( "DCUTM", 1.*_KeV );    /** delta-rays by muons (GeV)*/
  gMC->SetCut( "PPCUTM", 1.*_KeV );   /** direct pair production by muons (GeV)*/
  gMC->SetCut( "TOFMAX", tofmax ); /**time of flight cut in seconds*/
  
   
}
