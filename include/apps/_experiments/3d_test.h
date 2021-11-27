#ifndef OSW_APP_3D_TEST_H
#define OSW_APP_3D_TEST_H

#include <osw_hal.h>

#include "osw_app.h"

class OswApp3DTest : public OswApp {
 public:
  OswApp3DTest(void){};
  virtual void setup() override;
  virtual void loop() override;
  virtual void stop() override;
  ~OswApp3DTest(){};

 private:
};

#endif