// Ecole supérieure SL229_MINF TP
// Manipulation Tp3_MenuGen
// Créé le 9 Mai 2006 CHR
// Version 2016  du 03.02.2016
// Modif 2015 : utilisation de stdint.h
// Modif 2016 : ajout Duty pour PWM
// Modif 2018 SCA : suppression PWM et duty
// Definition pour le menuGen

#ifndef DEFMENUGEN_H
#define DEFMENUGEN_H

#include <stdint.h>
#include <stdbool.h>
#define MAGIC 0x123455AA



typedef enum  { SignalSinus, SignalTriangle, SignalDentDeScie, SignalCarre } E_FormesSignal;

// Structure des paramètres du générateur
typedef struct {
      E_FormesSignal Forme; //32bits / 4 bytes
      int16_t Frequence;    //16bits / 2 bytes
      int16_t Amplitude;    //16bits / 2 bytes
      int16_t Offset;       //16bits / 2 bytes
      bool FlagTcpip;
      uint32_t Magic;       //32bits / 4 bytes
      
} S_ParamGen;               //TOTAL : 14 bytes

#endif
