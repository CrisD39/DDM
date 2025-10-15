// spc.h
#pragma once
#include "qek.h"

class SPC : public QEK {
public:
    // Sobreescribí solo lo que uses
    void execute20() override;
    void execute21() override;
    void execute22() override;
};
