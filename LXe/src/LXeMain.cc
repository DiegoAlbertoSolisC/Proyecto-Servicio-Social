
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"

#include "LXeDetectorConstruction.hh"
#include "LXeActionInitialization.hh"

int main(int argc, char** argv) {
  auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::MT);
  runManager->SetNumberOfThreads(1);

  // Detector
  auto detector = new LXeDetectorConstruction;
  runManager->SetUserInitialization(detector);

  // Física + óptica
  auto physicsList = new FTFP_BERT;
  physicsList->RegisterPhysics(new G4OpticalPhysics()); // Cherenkov
  runManager->SetUserInitialization(physicsList);

  // Acciones
  runManager->SetUserInitialization(new LXeActionInitialization(detector));

  // Visualización
  auto visManager = new G4VisExecutive;
  visManager->Initialize();

  auto uiManager = G4UImanager::GetUIpointer();

  if (argc == 1) {
    auto ui = new G4UIExecutive(argc, argv);
    uiManager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
    delete ui;
  } else {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    uiManager->ApplyCommand(command + fileName);
  }

  delete visManager;
  delete runManager;
  return 0;
}
