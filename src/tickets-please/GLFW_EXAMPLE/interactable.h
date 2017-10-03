#ifndef INTERACTABLE_H
#define INTERACTABLE_H

//class to define models that the player can interact with
// such as NPCs and any devices

#include "model.h"

using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Interactable : public Model
{
public:


private:

}
#endif
