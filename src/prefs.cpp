
#include "prefs.h"


Preferences preferences;

void prefs_setup()
{
    preferences.begin("minicar", false);
    DEBUG_PRINTLN("[PREFS]. OK");
}





