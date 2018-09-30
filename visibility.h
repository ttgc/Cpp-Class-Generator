#ifndef VISIBILITY_H
#define VISIBILITY_H

#include <string>

enum Visibility {PUBLIC=0,PROTECTED=1,PRIVATE=2};
std::string translate_visibility(Visibility visi);

#endif // VISIBILITY_H
