#include <sound/soundrenderer/soundrenderingmanager.h>

namespace Sound {

void daSoundRenderingManager::RunMargeSoundScripts( void )
{
    SetCurrentNameSpace( GetCharacterNamespace( SC_CHAR_MARGE ) );
    GetSoundManager()->GetSoundLoader()->SetCurrentCluster( SC_CHAR_MARGE );
    #include "marge.inl"
}

}
