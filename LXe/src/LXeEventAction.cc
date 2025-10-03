#include "LXeEventAction.hh"
#include "LXeDetectorConstruction.hh"
#include "LXePMTHit.hh"
#include "LXeHistoManager.hh"
#include "LXeEventMessenger.hh"

#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

LXeEventAction::LXeEventAction(const LXeDetectorConstruction* det)
  : fDetector(det),
    fMessenger(nullptr),
    fHistoManager(LXeHistoManager::GetInstance()),
    fPMTCollID(-1),
    fPrimaryEnergy(0.),
    fPrimaryPosition(0., 0., 0.),
    fVerboseLevel(0),
    fPMTThreshold(0),
    fForceDrawPhotons(false),
    fForceDrawNoPhotons(false),
    fPhotonCount_Scint(0),
    fPhotonCount_Ceren(0),
    fAbsorptionCount(0),
    fBoundaryAbsorptionCount(0),
    fConvPos(0.,0.,0.),
    fConvPosSet(false)
{
  fMessenger = new LXeEventMessenger(this);
}

LXeEventAction::~LXeEventAction()
{
  delete fMessenger;
}

void LXeEventAction::BeginOfEventAction(const G4Event*)
{
  fPhotonCount_Scint = 0;
  fPhotonCount_Ceren = 0;
  fAbsorptionCount = 0;
  fBoundaryAbsorptionCount = 0;
  fConvPosSet = false;
}

void LXeEventAction::EndOfEventAction(const G4Event* event)
{
  if (fPMTCollID == -1) {
    fPMTCollID = G4SDManager::GetSDMpointer()
                   ->GetCollectionID("pmtHitCollection");
  }

  bool wroteRow = false;
  G4int nEntries = -1;

  if (fPMTCollID >= 0) {
    G4HCofThisEvent* hce = event->GetHCofThisEvent();
    if (hce) {
      auto pmtHC = static_cast<LXePMTHitsCollection*>(hce->GetHC(fPMTCollID));
      if (pmtHC) {
        nEntries = pmtHC->entries();
        G4cout << "[Debug] PMT-hits entries = " << nEntries << G4endl;
        const G4int eventID = event->GetEventID();
        for (G4int i = 0; i < nEntries; ++i) {
          LXePMTHit* hit = (*pmtHC)[i];
          const G4ThreeVector pos = hit->GetPMTPos();
          const G4int pmtID = hit->GetPMTNumber();

          fHistoManager->SaveHitToCSV(eventID, pmtID,
                                      pos.x()/mm, pos.y()/mm, pos.z()/mm,
                                      fPrimaryEnergy/MeV,
                                      fPrimaryPosition.x()/mm,
                                      fPrimaryPosition.y()/mm,
                                      fPrimaryPosition.z()/mm);
          wroteRow = true;
        }
      }
    }
  }

  if (!wroteRow && fVerboseLevel > 1) {
    G4cout << "[Debug] No PMT rows written for event "
           << event->GetEventID() << G4endl;
  }

  if (fVerboseLevel > 0) {
    G4cout << "Evt " << event->GetEventID()
           << " scint/Ceren photons: " << fPhotonCount_Scint << "/"
           << fPhotonCount_Ceren
           << " absorptions(boundary): " << fAbsorptionCount
           << "(" << fBoundaryAbsorptionCount << ")" << G4endl;
  }
}

void LXeEventAction::SetPrimaryEnergy(G4double e)         { fPrimaryEnergy = e; }
void LXeEventAction::SetPrimaryPosition(const G4ThreeVector& pos) { fPrimaryPosition = pos; }
void LXeEventAction::SetEventVerbose(G4int v)             { fVerboseLevel = v; }
void LXeEventAction::SetPMTThreshold(G4int thr)           { fPMTThreshold = thr; }
void LXeEventAction::SetForceDrawPhotons(G4bool v)        { fForceDrawPhotons = v; }
void LXeEventAction::SetForceDrawNoPhotons(G4bool v)      { fForceDrawNoPhotons = v; }

void LXeEventAction::IncPhotonCount_Scint()               { ++fPhotonCount_Scint; }
void LXeEventAction::IncPhotonCount_Ceren()               { ++fPhotonCount_Ceren; }
void LXeEventAction::IncAbsorption()                      { ++fAbsorptionCount; }
void LXeEventAction::IncBoundaryAbsorption()              { ++fBoundaryAbsorptionCount; }

void LXeEventAction::SetConvPos(const G4ThreeVector& p)   { fConvPos = p; fConvPosSet = true; }
G4bool LXeEventAction::IsConvPosSet() const               { return fConvPosSet; }