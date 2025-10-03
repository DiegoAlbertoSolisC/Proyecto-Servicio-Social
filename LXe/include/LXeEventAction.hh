#ifndef LXeEventAction_h
#define LXeEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

// Forward declarations
class LXeDetectorConstruction;
class LXeHistoManager;
class LXeEventMessenger;
class G4Event;

/**
 * Event action that:
 *   • collects PMT hits and writes them to CSV via LXeHistoManager
 *   • keeps bookkeeping counters for various photon statistics
 *   • offers setters used by LXeEventMessenger
 */
class LXeEventAction : public G4UserEventAction {
public:
  explicit LXeEventAction(const LXeDetectorConstruction* det);
  ~LXeEventAction() override;

  // Geant4 hooks
  void BeginOfEventAction(const G4Event*) override;
  void EndOfEventAction  (const G4Event*) override;

  // ------------ interface used by other classes ----------
  // from PrimaryGenerator
  void SetPrimaryEnergy(G4double energy);
  void SetPrimaryPosition(const G4ThreeVector& pos);

  // from Stacking & Stepping
  void IncPhotonCount_Scint();
  void IncPhotonCount_Ceren();
  void IncAbsorption();
  void IncBoundaryAbsorption();
  void SetConvPos(const G4ThreeVector& pos);
  G4bool IsConvPosSet() const;
  const G4ThreeVector& GetConvPos() const { return fConvPos; }

  // from Messenger (user UI)
  void SetEventVerbose   (G4int level);
  void SetPMTThreshold   (G4int thr);
  void SetForceDrawPhotons   (G4bool v);
  void SetForceDrawNoPhotons (G4bool v);

  // simple getters
  G4int  GetEventVerbose()   const { return fVerboseLevel; }
  G4int  GetPMTThreshold()   const { return fPMTThreshold; }
  G4bool GetForceDrawPhotons() const { return fForceDrawPhotons; }
  G4bool GetForceDrawNoPhotons() const { return fForceDrawNoPhotons; }
  G4int  GetPhotonCount_Scint() const { return fPhotonCount_Scint; }
  G4int  GetPhotonCount_Ceren() const { return fPhotonCount_Ceren; }
  G4int  GetAbsorptionCount() const { return fAbsorptionCount; }
  G4int  GetBoundaryAbsorptionCount() const { return fBoundaryAbsorptionCount; }
  const G4ThreeVector& GetPrimaryPosition() const { return fPrimaryPosition; }

private:
  const LXeDetectorConstruction* fDetector;     //!< not owned
  LXeEventMessenger*   fMessenger;              //!< UI cmd handler (owned)
  LXeHistoManager*     fHistoManager;           //!< singleton, not deleted
  G4int                fPMTCollID;              //!< cached hits‑collection ID

  // CSV data
  G4double fPrimaryEnergy;                      //!< kinetic energy of the primary
  G4ThreeVector fPrimaryPosition;               //!< position of the primary particle

  // UI‑controlled knobs
  G4int  fVerboseLevel;     // 0 = silent
  G4int  fPMTThreshold;     // min photons/hit
  G4bool fForceDrawPhotons; // not used here, but kept for compatibility
  G4bool fForceDrawNoPhotons;

  // per‑event bookkeeping
  G4int  fPhotonCount_Scint;
  G4int  fPhotonCount_Ceren;
  G4int  fAbsorptionCount;
  G4int  fBoundaryAbsorptionCount;

  G4ThreeVector fConvPos;
  G4bool        fConvPosSet;
};

#endif