#include "LXeHistoManager.hh"

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

G4Mutex LXeHistoManager::fPrintMutex = G4MUTEX_INITIALIZER;

LXeHistoManager* LXeHistoManager::GetInstance()
{
  // Construct-On-First-Use pattern, thread‑local
  static G4ThreadLocal LXeHistoManager instance;
  return &instance;
}

LXeHistoManager::LXeHistoManager()
{
  OpenCSV();
}

LXeHistoManager::~LXeHistoManager()
{
  CloseCSV();
}

void LXeHistoManager::OpenCSV()
{
  const int tid = G4Threading::G4GetThreadId();
  std::ostringstream fn;
  fn << "pmt_hits_t" << tid << ".csv";
  fFileName = fn.str();

  // trunc → siempre empezamos con archivo limpio
  fCSV.open(fFileName,
            std::ios::out | std::ios::trunc);

  G4AutoLock lock(&fPrintMutex);
  if (fCSV.is_open()) {
    std::cout << "[CSV] Abierto/creado: "
              << std::filesystem::absolute(fFileName) << '\n';

    // Cabecera actualizada con posición primaria
    fCSV << "eventID,pmtID,x(mm),y(mm),z(mm),energy(MeV),x0(mm),y0(mm),z0(mm)\n";
  } else {
    std::cerr << "[CSV] ¡No se pudo abrir "
              << std::filesystem::absolute(fFileName) << "!\n";
  }
}

void LXeHistoManager::CloseCSV()
{
  if (fCSV.is_open()) {
    fCSV.flush();
    fCSV.close();
    G4AutoLock lock(&fPrintMutex);
    std::cout << "[CSV] Cerrado " << fFileName << '\n';
  }
}

void LXeHistoManager::SaveHitToCSV(G4int evt, G4int pmt,
                                   G4double x, G4double y, G4double z,
                                   G4double e,
                                   G4double x0, G4double y0, G4double z0)
{
  if (!fCSV.is_open()) return;

  fCSV << evt << ',' << pmt << ','
       << std::fixed << std::setprecision(1)
       << x << ',' << y << ',' << z << ','
       << std::setprecision(4) << e << ','
       << x0 << ',' << y0 << ',' << z0 << '\n';

  // vaciamos inmediatamente para garantizar que llega al disco
  fCSV.flush();

  // Mensaje de depuración 
  {
    G4AutoLock lock(&fPrintMutex);
    std::cout << "[CSV] evt=" << evt
              << " pmt=" << pmt
              << " x=" << x
              << " y=" << y
              << " z=" << z
              << " E=" << e
              << " x0=" << x0
              << " y0=" << y0
              << " z0=" << z0
              << "  (archivo=" << fFileName << ")\n";
  }
}
