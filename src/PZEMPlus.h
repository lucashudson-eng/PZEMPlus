#ifndef PZEMPLUS_H
#define PZEMPLUS_H

#if defined(PZEM_004T)
#include "PZEM004T.h"
typedef PZEM004T PZEMPlus;

#elif defined(PZEM_003) || defined(PZEM_017)
#include "PZEM003017.h"
typedef PZEM003017 PZEMPlus;

#elif defined(PZEM_6L24)
#include "PZEM6L24.h"
typedef PZEM6L24 PZEMPlus;

#elif defined(PZEM_IOT_E02)
#include "PZEMIOTE02.h"
typedef PZEMIOTE02 PZEMPlus;

#else
#error "Please define Peacefair PZEM model"
#endif

#endif // PZEMPLUS_H
