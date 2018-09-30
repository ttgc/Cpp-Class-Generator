#include "visibility.h"

using namespace std;

string translate_visibility(Visibility visi)
{
    switch (visi) {
    case PUBLIC:
        return "public";
    case PROTECTED:
        return "protected";
    case PRIVATE:
        return "private";
    }
    return "";
}
