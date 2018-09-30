#ifndef ANCESTOR_H
#define ANCESTOR_H

#include <QString>
#include <visibility.h>

class Ancestor
{
    public:
        Ancestor(QString const& ancestor);
        Ancestor(QString const& ancestor, Visibility visi, QString const& import);
        ~Ancestor();
        QString* getName() const {return name;}
        Visibility getVisibility() const {return visibility;}
        QString* getFile() const {return file;}

    protected:
        QString* name;
        Visibility visibility;
        QString* file;
};

#endif // ANCESTOR_H
