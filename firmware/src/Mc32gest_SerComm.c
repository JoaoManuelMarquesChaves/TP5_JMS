// Mc32Gest_SerComm.C
// fonction d'émission et de réception des message
// transmis en USB CDC
// Canevas TP4 SLO2 2015-2015


#include "app.h"
#include "Mc32gest_SerComm.h"
#include "appgen.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


S_ParamGen RemoteParamGen;

// Fonction de reception  d'un  message
// Met à jour les paramètres du generateur a partir du message recu
// Format du message
//  !S=TF=2000A=10000O=+5000D=100W=0#
//  !S=PF=2000A=10000O=-5000D=100W=1#


//le format du message doit être fixe : !S=Tf=2000A=01000O=+5000W=0#
bool GetMessage(char *Data)
{
    //afficher un caractère recu 
     E_FormesSignal Forme_recue=0;
    int16_t Frequence_recue = 0;
    int16_t Amplitude_recue = 0;
    int16_t Offset_recu = 0;
    bool SaveTodo;
    static bool SaveTodo_Old = false;
    bool FlagGet = 0;

//controler que le premier caratere est un "!"
   if (Data[0] == '!')
   {
              // Traduction de la forme du signal
        switch (Data[3])
        {
            case 'T':

                Forme_recue = SignalTriangle;

                break;

            case 'S':

                Forme_recue = SignalSinus;

                break;

            case 'C':

                Forme_recue = SignalCarre;

                break;

            case 'D':

                Forme_recue = SignalDentDeScie;

                break;
        }

        
        // Traduction de la frequence
        Frequence_recue = atoi(&Data[6]);
        
        // Traduction de l'amplitude
        Amplitude_recue = atoi(&Data[12]);

        // Traduction de l'offset
        Offset_recu = atoi(&Data[19]);
        
        //traduir la save
        SaveTodo = atoi(&Data[26]);
        //tester si les caratère on ete modifier durant les deux envoie
        if((Offset_recu == RemoteParamGen.Offset) && (Amplitude_recue == RemoteParamGen.Amplitude) && (Frequence_recue == RemoteParamGen.Frequence) && (Forme_recue == RemoteParamGen.Forme )&& (SaveTodo_Old == SaveTodo))
        {
            //si ce n'est pas le cas mettre le Flag remote à 0
            FlagRemote_Val(false);
            FlagGet = 0;
        }
        else 
        {
            //sinon le Flag remote à 1
            FlagRemote_Val(true);
            FlagGet = 1;
        }
        
        // Mise a jour des parametres de pParam
        RemoteParamGen.Forme = Forme_recue;
        RemoteParamGen.Frequence = Frequence_recue;
        RemoteParamGen.Amplitude = Amplitude_recue;
        RemoteParamGen.Offset = Offset_recu;
        SaveTodo_Old = SaveTodo;
        
        
        if (SaveTodo != 0)
        {
            FlagSave_Val(true);
        }
        else 
        {
            FlagSave_Val(false);
        }
    }   
    return(FlagGet);
} // GetMessage

void SendMessage(char *AppBuffer, int16_t Size)
{
    char Forme = 'T';

    if(FlagRemote_State())
    {
        switch (RemoteParamGen.Forme)
        {
            case 0:

                Forme = 'S';

                break;

            case 1:

                Forme = 'T';

                break;

            case 2:

                Forme = 'D';

                break;

            case 3:

                Forme = 'C';

                break;
        }
        //SendMessage((char*)AppBuffer);
        sprintf((char*)AppBuffer,"!S=%cf=%dA=%dO=%+dWP=%d#",Forme, RemoteParamGen.Frequence,RemoteParamGen.Amplitude, RemoteParamGen.Offset, FlagSave_State());
        
//        if ( FlagRemote_State())
//        {
//            //Transfer the data out of our local processing buffer and into the TCP TX FIFO.
//            SYS_CONSOLE_PRINT("Server Sending %s\r\n", AppBuffer);
//        }   
    }
} // SendMessage
