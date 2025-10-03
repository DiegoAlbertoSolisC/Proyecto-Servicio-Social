#ifndef LXeRunAction_h
#define LXeRunAction_h 1

#include "G4UserRunAction.hh"

/** Une los CSV de cada hilo en uno solo y elimina los intermedios. */
class LXeRunAction : public G4UserRunAction
{
public:
  LXeRunAction();
  ~LXeRunAction() override;

  void BeginOfRunAction(const G4Run*) override;
  void EndOfRunAction  (const G4Run*) override;
};

#endif
