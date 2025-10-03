#ifndef LXeHistoManager_h
#define LXeHistoManager_h 1

#include "globals.hh"
#include "G4Threading.hh"
#include "G4AutoLock.hh"

#include <fstream>
#include <mutex>
#include <string>

/**  Thread‑local histogram/CSV manager.
 *   Cada hilo de Geant4 escribe en su propio fichero pmt_hits_t<N>.csv
 *   y la instancia se destruye automáticamente al terminar el hilo,
 *   cerrando el flujo de forma segura.
 */
class LXeHistoManager {
public:
  /** Única manera de obtener la instancia del hilo actual. */
  static LXeHistoManager* GetInstance();

  /** Escribe una fila de hit en el CSV y fuerza flush inmediato. */
  void SaveHitToCSV(G4int eventID, G4int pmtID,
                    G4double x_mm, G4double y_mm, G4double z_mm,
                    G4double energy_MeV,
                    G4double x0_mm, G4double y0_mm, G4double z0_mm);

private:
  LXeHistoManager();              // abre el archivo
  ~LXeHistoManager();             // cierra el archivo

  void OpenCSV();
  void CloseCSV();

  std::ofstream fCSV;
  std::string   fFileName;

  // Mutex global para mensajes de depuración (cout)
  static G4Mutex fPrintMutex;
};

#endif
